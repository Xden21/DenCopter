using DencopterMonitoring.Domain;
using NLog;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(IDataService))]
    public class DataService : IDataService
    {
        #region NLog

        private static Logger Logger = LogManager.GetCurrentClassLogger();

        #endregion

        private readonly ISettingsService settingsService;
        private readonly IGeneralService generalService;


        [ImportingConstructor]
        public DataService(ISettingsService settingsService,
            IGeneralService generalService)
        {
            this.settingsService = settingsService;
            this.generalService = generalService;
            dataLock = new object();
            AllDataSets = new List<DataSet>();
        }

        public event EventHandler<DataUpdateEventArgs> DataUpdateEvent;

        public event EventHandler<PIDUpdateEventArgs> PIDDataUpdateEvent;

        public void TriggerDataUpdateEvent(List<DataSet> dataSets)
        {
            DataUpdateEvent?.Invoke(this, new DataUpdateEventArgs(dataSets));
        }
        
        public void TriggerPIDDataUpdateEvent(PIDData data)
        {
            PIDDataUpdateEvent?.Invoke(this, new PIDUpdateEventArgs(data));
        }

        /*
        * List of datasets that haven't been processed.
        */
        public List<DataSet> UnprocessedDataSets { get; set; }

        /*
         * List of all received datasets
         */
        public List<DataSet> AllDataSets { get; set; }

        private object dataLock;

        public object DataLock { get => dataLock; }

        #region Methods

        public void SaveLoggingToDisk()
        {
            try
            {
                if (AllDataSets.Count != 0)
                {
                    StringBuilder builder = new StringBuilder("Mode;time;yaw estimate; pitch estimate; roll estimate;yaw measured; pitch measured; roll measured;yaw desired; pitch desired; roll desired; x-axis; y-axis; z-axis; motor1;motor2;motor3;motor4; altitude estimate ; altitude measured; altitude desired ; x-pos estimate ; y-pos estimate ; x-pos measured ; y-pos measured ; x-pos target ; y-pos target ; x-vel estimate ; y-vel estimate\n");
                    lock (AllDataSets)
                    {
                        foreach (DataSet set in AllDataSets)
                        {
                            builder.Append(set.ToString());
                        }
                    }
                    string time = DateTime.Now.ToString("ddMMyy-HHmmss");

                    System.IO.File.WriteAllText(settingsService.SaveFolder + @"\AllLoggerData" + time + ".csv", builder.ToString());
                }
            }
            catch (Exception ex)
            {
                Logger.Error(ex, "Save Failed");
            }
        }

        public void Reset()
        {
            SaveLoggingToDisk();
            AllDataSets = new List<DataSet>();
        }

        #endregion
    }

    public class DataUpdateEventArgs : EventArgs
    {
        public DataUpdateEventArgs(List<DataSet> dataSets)
        {
            DataSets = dataSets;
        }

        public List<DataSet> DataSets { get; set; }
    }

    public class PIDUpdateEventArgs: EventArgs
    {
        public PIDUpdateEventArgs(PIDData data)
        {
            PIDData = data;
        }

        public PIDData PIDData { get; set; }
    }
}
