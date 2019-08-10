using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.ViewModels;
using DencopterMonitoring.Domain;
using NLog;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Controllers
{
    [Export]
    public class AngleMonitoringController
    {
        #region NLog

        private static Logger Logger = LogManager.GetCurrentClassLogger();

        #endregion

        #region Fields

        private readonly AngleMonitoringViewModel monitoringViewModel;
        private readonly IShellService shellService;
        private readonly ISettingsService settingsService;
        private readonly IGeneralService generalService;
        private readonly IDataService dataService;

        private bool isVisible;
        private bool attUpdating;

        private bool buffersFull;
        List<LogPoint> rollMeasBuffer;
        List<LogPoint> rollRefBuffer;

        List<LogPoint> pitchMeasBuffer;
        List<LogPoint> pitchRefBuffer;

        List<LogPoint> yawMeasBuffer;
        List<LogPoint> yawRefBuffer;

        #endregion

        #region Constuctor

        [ImportingConstructor]
        public AngleMonitoringController(AngleMonitoringViewModel monitoringViewModel, IShellService shellService, ISettingsService settingsService, IGeneralService generalService,
            IDataService dataService)
        {
            this.monitoringViewModel = monitoringViewModel;
            this.shellService = shellService;
            this.settingsService = settingsService;
            this.generalService = generalService;

            attUpdating = false;

            buffersFull = false;

            rollMeasBuffer = new List<LogPoint>();
            rollRefBuffer = new List<LogPoint>();

            pitchMeasBuffer = new List<LogPoint>();
            pitchRefBuffer = new List<LogPoint>();

            yawMeasBuffer = new List<LogPoint>();
            yawRefBuffer = new List<LogPoint>();

            isVisible = true;

            this.dataService = dataService;
            dataService.DataUpdateEvent += DataUpdateEventHandler;
        }

        #endregion

        #region Methods

        public void Initialize()
        {
            shellService.AngleMonitoringViewModel = monitoringViewModel;
            shellService.PropertyChanged += ResumeUpdateCheck;
        }

        private void ResumeUpdateCheck(object sender, PropertyChangedEventArgs args)
        {
            if(args.PropertyName == "AngleVisible")
            {
                if (shellService.AngleVisible)
                    isVisible = true;
                else
                    isVisible = false;
            }
        }

        private void DataUpdateEventHandler(object sender, DataUpdateEventArgs args)
        {
            if (args.DataSets == null || args.DataSets.Count == 0)
                return; // No updating to be done

            generalService.FlightMode = args.DataSets.Last().FlightMode; //Set flight mode to most up to date value

            if (isVisible)
            {
                if (attUpdating)
                {
                    UpdateAttitudeBuffer(args.DataSets);
                }
                else
                {
                    attUpdating = true;
                    ThreadPool.QueueUserWorkItem((x) =>
                    {
                        UpdateAttitude(args.DataSets);
                        attUpdating = false;
                    });
                }
            }
            else
            {
                ThreadPool.QueueUserWorkItem((x) =>
                {
                    UpdateAttitudeBuffer(args.DataSets);
                });
            }
        }

        private void UpdateAttitude(List<DataSet> dataSets)
        {
            try
            {
                bool addBuffer;
                if (rollMeasBuffer.Count > 0)
                    addBuffer = true;
                else
                    addBuffer = false;

                if (dataSets == null || dataSets.Count == 0)
                {
                    return;
                }

                if (addBuffer)
                {
                    UpdateAttitudeBuffer(dataSets);
                    if (buffersFull)
                    {
                        monitoringViewModel.RollMeasPoints.Clear();
                        monitoringViewModel.RollRefPoints.Clear();

                        monitoringViewModel.PitchMeasPoints.Clear();
                        monitoringViewModel.PitchRefPoints.Clear();

                        monitoringViewModel.YawMeasPoints.Clear();
                        monitoringViewModel.YawRefPoints.Clear();
                    }
                    else
                    {
                        int index = 0;
                        while (monitoringViewModel.RollMeasPoints.Count > 0 && index < monitoringViewModel.RollMeasPoints.Count - 1 && (rollMeasBuffer.Last().TimeVal - monitoringViewModel.RollMeasPoints[index].TimeVal) > settingsService.TimeFrame)
                            index++;

                        for (int i = 0; i < index; i++)
                        {
                            monitoringViewModel.RollMeasPoints.RemoveAt(0);
                            monitoringViewModel.RollRefPoints.RemoveAt(0);

                            monitoringViewModel.PitchMeasPoints.RemoveAt(0);
                            monitoringViewModel.PitchRefPoints.RemoveAt(0);

                            monitoringViewModel.YawMeasPoints.RemoveAt(0);
                            monitoringViewModel.YawRefPoints.RemoveAt(0);
                        }
                    }
                    monitoringViewModel.RollMeasPoints.AddRange(rollMeasBuffer);
                    monitoringViewModel.RollRefPoints.AddRange(rollRefBuffer);

                    monitoringViewModel.PitchMeasPoints.AddRange(pitchMeasBuffer);
                    monitoringViewModel.PitchRefPoints.AddRange(pitchRefBuffer);

                    monitoringViewModel.YawMeasPoints.AddRange(yawMeasBuffer);
                    monitoringViewModel.YawRefPoints.AddRange(yawRefBuffer);

                    rollMeasBuffer.Clear();
                    rollRefBuffer.Clear();

                    pitchMeasBuffer.Clear();
                    pitchRefBuffer.Clear();

                    yawMeasBuffer.Clear();
                    yawRefBuffer.Clear();
                    buffersFull = false;
                }
                else
                {
                    int index = 0; // First index which stays
                    while (index < monitoringViewModel.RollMeasPoints.Count - 1 && (dataSets.Last().TimeStamp - monitoringViewModel.RollMeasPoints[index].TimeVal) > settingsService.TimeFrame)
                        index++;

                    for (int i = 0; i < index; i++)
                    {
                        monitoringViewModel.RollMeasPoints.RemoveAt(0);
                        monitoringViewModel.RollRefPoints.RemoveAt(0);

                        monitoringViewModel.PitchMeasPoints.RemoveAt(0);
                        monitoringViewModel.PitchRefPoints.RemoveAt(0);

                        monitoringViewModel.YawMeasPoints.RemoveAt(0);
                        monitoringViewModel.YawRefPoints.RemoveAt(0);
                    }
                    List<LogPoint> newRollMeasPoints = new List<LogPoint>();
                    List<LogPoint> newRollRefPoints = new List<LogPoint>();

                    List<LogPoint> newPitchMeasPoints = new List<LogPoint>();
                    List<LogPoint> newPitchRefPoints = new List<LogPoint>();

                    List<LogPoint> newYawMeasPoints = new List<LogPoint>();
                    List<LogPoint> newYawRefPoints = new List<LogPoint>();

                    foreach (DataSet dataSet in dataSets)
                    {
                        newRollMeasPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleMeasured.Roll));
                        newRollRefPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleReference.Roll));

                        newPitchMeasPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleMeasured.Pitch));
                        newPitchRefPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleReference.Pitch));

                        newYawMeasPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleMeasured.Yaw));
                        newYawRefPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleReference.Yaw));
                    }

                    monitoringViewModel.RollMeasPoints.AddRange(newRollMeasPoints);
                    monitoringViewModel.RollRefPoints.AddRange(newRollRefPoints);

                    monitoringViewModel.PitchMeasPoints.AddRange(newPitchMeasPoints);
                    monitoringViewModel.PitchRefPoints.AddRange(newPitchRefPoints);

                    monitoringViewModel.YawMeasPoints.AddRange(newYawMeasPoints);
                    monitoringViewModel.YawRefPoints.AddRange(newYawRefPoints);
                }
            }
            catch (Exception ex)
            {
                Logger.Error(ex, "Update Attitude Failed");
            }
        }

        private void UpdateAttitudeBuffer(List<DataSet> dataSets)
        {
            if (dataSets == null || dataSets.Count == 0)
                return; // No updating to be done

            float firstTime = (float)rollMeasBuffer.DefaultIfEmpty(new LogPoint(0, 0)).First().TimeVal;

            foreach (DataSet dataSet in dataSets)
            {
                if (firstTime != 0 && (rollMeasBuffer.Count > 0) && (dataSet.TimeStamp - firstTime) > settingsService.TimeFrame)
                {
                    buffersFull = true;

                    rollMeasBuffer.RemoveAt(0);
                    pitchMeasBuffer.RemoveAt(0);
                    yawMeasBuffer.RemoveAt(0);
                    rollRefBuffer.RemoveAt(0);
                    pitchRefBuffer.RemoveAt(0);
                    yawRefBuffer.RemoveAt(0);

                    firstTime = (float)rollMeasBuffer.DefaultIfEmpty(new LogPoint(0, 0)).First().TimeVal;
                }

                rollMeasBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleMeasured.Roll));
                rollRefBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleReference.Roll));

                pitchMeasBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleMeasured.Pitch));
                pitchRefBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleReference.Pitch));

                yawMeasBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleMeasured.Yaw));
                yawRefBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.AngleReference.Yaw));
            }
        }

        #endregion

    }
}
