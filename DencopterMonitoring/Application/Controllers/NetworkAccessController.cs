using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Domain;
using NLog;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Controllers
{
    public class NetworkAccessController
    {
        #region NLog

        private static Logger Logger = LogManager.GetCurrentClassLogger();

        #endregion

        #region Vars & Props

#if OFFLINE
        double time;
        float val;
#endif

        private IGeneralService generalService;
        private ISettingsService settingsService;
        private IDataService dataFetchService;
        private TcpClient connection;
        private NetworkStream stream;
        private Thread NetworkThread;

        private bool connecting;

        #endregion

        #region Constructor

        [ImportingConstructor]
        public NetworkAccessController(IGeneralService generalService, ISettingsService settingsService, IDataService dataFetchService)
        {
            this.generalService = generalService;
            this.settingsService = settingsService;
            this.dataFetchService = dataFetchService;
            dataFetchService.UnprocessedDataSets = new List<DataSet>();
            generalService.PropertyChanged += GeneralService_PropertyChanged;
            connecting = false;
#if OFFLINE
            time = 0;
            val = 0;
#endif
        }

        #endregion              

        #region Methods

        public void TryConnect()
        {
            if (!connecting)
            {
                connecting = true;
                while (!generalService.Connected && !generalService.Disconnect)
                {
                    Connect();
                }
                Logger.Info("Connected");
                connecting = false;
            }
        }

        private bool Connect()
        {
#if !OFFLINE
            try
            {
                if (connection != null)
                {
                    connection.Close();
                    Thread.Sleep(500);
                }
                connection = new TcpClient(settingsService.InetAddress, settingsService.Port);
                stream = connection.GetStream();
                byte[] id = Encoding.UTF8.GetBytes("DencopterClient");
                stream.Write(id, 0, id.Length);
                byte[] response = new byte[1];
                stream.Read(response, 0, 1);
                if(response[0] != 1)
                {
                    if (response[0] == 2)
                        Logger.Warn("Bad ID!");
                    else if (response[0] == 3)
                        Logger.Warn("No Client Slot available");
                    else
                        Logger.Warn("Unknown Connection Response");
                    return false;
                }
                generalService.Connected = true;
                if (NetworkThread != null && NetworkThread.ThreadState == ThreadState.Running)
                {
                    NetworkThread.Abort();
                }
                NetworkThread = new Thread(NetworkThreadRun);
                NetworkThread.Start();
                return true;
            }
            catch (Exception ex)
            {
                Logger.Error(ex, "Connecting Failed");
                Thread.Sleep(200);
                return false;
            }
#else

            generalService.Connected = true;
            if (NetworkThread != null && NetworkThread.ThreadState == ThreadState.Running)
            {
                NetworkThread.Abort();
            }
            NetworkThread = new Thread(NetworkThreadRun);
            NetworkThread.Start();
            return true;
#endif
        }

        public void Disconnect()
        {
            if (generalService.Connected)
            {
#if !OFFLINE
                try
                {
                    byte[] data = Encoding.UTF8.GetBytes("end");
                    stream.Write(data, 0, data.Length);
                }
                catch (Exception)
                { }
                connection.Close();
#endif
                generalService.Connected = false;
                generalService.FlightMode = -1;
                connection = null;
            }
        }
        private void GeneralService_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Disconnect")
            {
                if (generalService.Disconnect)
                {
                    Disconnect();
                }
                else
                {
                    ThreadPool.QueueUserWorkItem((info) =>
                    {
                        TryConnect();
                    });
                }
            }
        }

        /**
         * Reads the network as fast as it gets data
         */
        private void NetworkThreadRun()
        {
            while (generalService.Connected)
            {
#if !OFFLINE
                try
                {
                    if (generalService.NewPIDData)
                    {
                        byte[] temp;
                        lock (generalService.PIDData)
                        {
                            temp = Encoding.UTF8.GetBytes(generalService.PIDData.ToString());
                        }
                        byte[] sendData = new byte[temp.Length + 1];
                        Array.Copy(temp, 0, sendData, 1, temp.Length);
                        sendData[0] = 11;
                        stream.Write(sendData, 0, sendData.Length);
                    }
                    else
                    {
                        byte[] sendData = new byte[1];
                        sendData[0] = 1;
                        stream.Write(sendData, 0, sendData.Length);
                    }                    

                    byte[] data;
                    byte[] tempdata = new byte[2000];
                    int amountRead = stream.Read(tempdata, 0, 2000);
                    data = new byte[amountRead];
                    Array.Copy(tempdata, data, amountRead);

                    DataSet fetchedData;
                    try
                    {
                        fetchedData = ParseMessage(data);
                    }
                    catch (Exception ex)
                    {
                        Logger.Error(ex, "Parse failed");
                        Thread.Sleep(50);
                        continue;
                    }
                    if (fetchedData.ValidInfo)
                    {
                        lock (dataFetchService.DataLock)
                        {
                            dataFetchService.UnprocessedDataSets.Add(fetchedData);
                        }
                    }
                    Thread.Sleep((int)(1.0 / settingsService.LoggerFreq * 1000.0));
                }
                catch (Exception ex)
                {
                    Logger.Error(ex, "Network Error");
                    Thread.Sleep(100);
                    if (connection != null && !connection.Connected && !generalService.Disconnect)
                    {
                        if (!Connect())
                            generalService.Connected = false;
                    }
                    continue;
                }
#else
                val = val + (float)(20.0 / 500.0);
                if (val > 20)
                    val = -20;

                time = time + 1.0 / 100.0;
                DataSet fetchedData = new DataSet
                {
                    ValidInfo = true,
                    FlightMode = 2,
                    TimeStamp = (float)time,
                    AngleMeasured = new EulerAngle { Roll = -val, Pitch = -val, Yaw = -val },
                    AngleReference = new EulerAngle { Roll = val/2, Pitch = val/2, Yaw = val/2 },
                    MotorSpeeds = new MotorSpeeds { MotorBL = val, MotorBR = val, MotorFL = val, MotorFR = val }
                };
                lock (dataFetchService.DataLock)
                {
                    dataFetchService.UnprocessedDataSets.Add(fetchedData);
                }
                Thread.Sleep(10);
#endif
            }
        }

        private DataSet ParseMessage(byte[] data)
        {
            string decodedData = Encoding.UTF8.GetString(data);
            string[] dataArray = decodedData.Split("|".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
            bool valid = true;
            bool newPIDData = false;
            PIDData pIDData = null;
            int flightMode = 3;
            int armed = 0;
            float timeStamp = -1000;
            EulerAngle AttMeasured = null, AttReference = null;
            MotorSpeeds motorSpeeds = null;
            if (dataArray[0] != "OK"  && dataArray[0] != "PID")
            {
                valid = false;
            }
            else
            {
                try
                {
                    if(dataArray[0] == "PID")
                    {
                        newPIDData = true;
                    }
                    // TODO FIX INDICES
                    flightMode = (int)Math.Round(float.Parse(dataArray[0]));
                    timeStamp = float.Parse(dataArray[1]);
                    armed = int.Parse(dataArray[2]);
                    AttMeasured = new EulerAngle
                    {
                        Yaw = float.Parse(dataArray[3]),
                        Pitch = float.Parse(dataArray[4]),
                        Roll = float.Parse(dataArray[5])
                    };
                    AttReference = new EulerAngle
                    {
                        Yaw = float.Parse(dataArray[6]),
                        Pitch = float.Parse(dataArray[7]),
                        Roll = float.Parse(dataArray[8])
                    };
                    motorSpeeds = new MotorSpeeds
                    {
                        MotorFL = float.Parse(dataArray[9]),
                        MotorFR = float.Parse(dataArray[10]),
                        MotorBL = float.Parse(dataArray[11]),
                        MotorBR = float.Parse(dataArray[12])
                    };               
                    if(newPIDData)
                    {
                        pIDData = new PIDData()
                        {
                            Yaw_KP = float.Parse(dataArray[13]),
                            Yaw_KI = float.Parse(dataArray[14]),
                            Yaw_KD = float.Parse(dataArray[15]),
                            Pitch_KP = float.Parse(dataArray[16]),
                            Pitch_KI = float.Parse(dataArray[17]),
                            Pitch_KD = float.Parse(dataArray[18]),
                            Roll_KP = float.Parse(dataArray[19]),
                            Roll_KI = float.Parse(dataArray[20]),
                            Roll_KD = float.Parse(dataArray[21])
                        };
                    }
                }
                catch (Exception ex)
                {
                    Logger.Error(ex, "Parse Failed, Message: " + String.Join(" ", dataArray));
                    valid = false;
                }
            }
            return new DataSet
            {
                ValidInfo = valid,
                PIDIncl = newPIDData,
                PIDData = pIDData,
                FlightMode = flightMode,
                TimeStamp = timeStamp,
                AngleMeasured = AttMeasured,
                AngleReference = AttReference,
                MotorSpeeds = motorSpeeds
            };
        }
        #endregion
    }
}

