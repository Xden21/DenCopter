using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.ViewModels;
using DencopterMonitoring.Domain;
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
    public class MotorMonitoringController
    {
        #region Fields

        private readonly MotorMonitoringViewModel motorMonitoringViewModel;
        private readonly IShellService shellService;
        private readonly ISettingsService settingsService;
        private readonly IGeneralService generalService;
        private readonly IDataService dataService;

        private bool isVisible;
        private bool motUpdating;

        private bool buffersFull;
        List<LogPoint> motorFLBuffer;
        List<LogPoint> motorFRBuffer;
        List<LogPoint> motorBLBuffer;
        List<LogPoint> motorBRBuffer;

        #endregion

        #region Constructor

        [ImportingConstructor]
        public MotorMonitoringController(MotorMonitoringViewModel motorMonitoringViewModel, IShellService shellService, ISettingsService settingsService, IGeneralService generalService,
            IDataService dataService)
        {
            this.motorMonitoringViewModel = motorMonitoringViewModel;
            this.shellService = shellService;
            this.settingsService = settingsService;
            this.generalService = generalService;

            motUpdating = false;

            buffersFull = false;

            motorFLBuffer = new List<LogPoint>();
            motorFRBuffer = new List<LogPoint>();

            motorBLBuffer = new List<LogPoint>();
            motorBRBuffer = new List<LogPoint>();

            isVisible = true;

            this.dataService = dataService;
            dataService.DataUpdateEvent += DataUpdateEventHandler;
        }

        #endregion

        #region Methods

        public void Initialize()
        {
            shellService.MotorMonitoringViewModel = motorMonitoringViewModel;
            shellService.PropertyChanged += ResumeUpdateCheck;
        }

        private void ResumeUpdateCheck(object sender, PropertyChangedEventArgs args)
        {
            if (args.PropertyName == "MotorVisible")
            {
                if (shellService.MotorVisible)
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
                if (motUpdating)
                {
                    UpdateMotorBuffer(args.DataSets);
                }
                else
                {
                    motUpdating = true;
                    ThreadPool.QueueUserWorkItem((x) =>
                    {
                        UpdateMotors(args.DataSets);
                        motUpdating = false;
                    });
                }
            }
            else
            {
                ThreadPool.QueueUserWorkItem((x) =>
                {
                    UpdateMotorBuffer(args.DataSets);
                });
            }
        }

        private void UpdateMotors(List<DataSet> dataSets)
        {
            bool addBuffer;
            if (motorFLBuffer.Count > 0)
                addBuffer = true;
            else
                addBuffer = false;

            if (dataSets == null || dataSets.Count == 0)
            {
                return;
            }

            if (addBuffer)
            {
                UpdateMotorBuffer(dataSets);
                if (buffersFull)
                {
                    // Buffer will delete all current values
                    motorMonitoringViewModel.MotorFLPoints.Clear();
                    motorMonitoringViewModel.MotorFRPoints.Clear();
                    motorMonitoringViewModel.MotorBLPoints.Clear();
                    motorMonitoringViewModel.MotorBRPoints.Clear();
                }
                else
                {
                    int index = 0;
                    while (motorMonitoringViewModel.MotorFLPoints.Count > 0 && index < motorMonitoringViewModel.MotorFLPoints.Count - 1 && (motorFLBuffer.Last().TimeVal - motorMonitoringViewModel.MotorFLPoints[index].TimeVal) > settingsService.TimeFrame)
                        index++;

                    for (int i = 0; i < index; i++)
                    {
                        motorMonitoringViewModel.MotorFLPoints.RemoveAt(0);
                        motorMonitoringViewModel.MotorFRPoints.RemoveAt(0);
                        motorMonitoringViewModel.MotorBLPoints.RemoveAt(0);
                        motorMonitoringViewModel.MotorBRPoints.RemoveAt(0);
                    }
                }
                motorMonitoringViewModel.MotorFLPoints.AddRange(motorFLBuffer);
                motorMonitoringViewModel.MotorFRPoints.AddRange(motorFRBuffer);
                motorMonitoringViewModel.MotorBLPoints.AddRange(motorBLBuffer);
                motorMonitoringViewModel.MotorBRPoints.AddRange(motorBRBuffer);
                motorFLBuffer.Clear();
                motorFRBuffer.Clear();
                motorBLBuffer.Clear();
                motorBRBuffer.Clear();
                buffersFull = false;
            }
            else
            {
                // Search which values to remove
                int index = 0; // First index which stays
                while (index < motorMonitoringViewModel.MotorFLPoints.Count - 1 && (dataSets.Last().TimeStamp - motorMonitoringViewModel.MotorFLPoints[index].TimeVal) > settingsService.TimeFrame)
                    index++;

                for (int i = 0; i < index; i++)
                {
                    motorMonitoringViewModel.MotorFLPoints.RemoveAt(0);
                    motorMonitoringViewModel.MotorFRPoints.RemoveAt(0);
                    motorMonitoringViewModel.MotorBLPoints.RemoveAt(0);
                    motorMonitoringViewModel.MotorBRPoints.RemoveAt(0);
                }
                List<LogPoint> newFLPoints = new List<LogPoint>();
                List<LogPoint> newFRPoints = new List<LogPoint>();
                List<LogPoint> newBLPoints = new List<LogPoint>();
                List<LogPoint> newBRPoints = new List<LogPoint>();

                foreach (DataSet dataSet in dataSets)
                {
                    newFLPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorFL));
                    newFRPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorFR));
                    newBLPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorBL));
                    newBRPoints.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorBR));
                }

                motorMonitoringViewModel.MotorFLPoints.AddRange(newFLPoints);
                motorMonitoringViewModel.MotorFRPoints.AddRange(newFRPoints);
                motorMonitoringViewModel.MotorBLPoints.AddRange(newBLPoints);
                motorMonitoringViewModel.MotorBRPoints.AddRange(newBRPoints);
            }
        }

        private void UpdateMotorBuffer(List<DataSet> dataSets)
        {
            if (dataSets == null || dataSets.Count == 0)
                return; // No updating to be done

            float firstTime = (float)motorFLBuffer.DefaultIfEmpty(new LogPoint(0, 0)).First().TimeVal;

            foreach (DataSet dataSet in dataSets)
            {
                if (firstTime != 0 && (motorFLBuffer.Count > 0) && (dataSet.TimeStamp - firstTime) > settingsService.TimeFrame)
                {
                    buffersFull = true;
                    motorFLBuffer.RemoveAt(0);
                    motorFRBuffer.RemoveAt(0);
                    motorBLBuffer.RemoveAt(0);
                    motorBRBuffer.RemoveAt(0);

                    firstTime = (float)motorFLBuffer.DefaultIfEmpty(new LogPoint(0, 0)).First().TimeVal;
                }
                motorFLBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorFL));
                motorFRBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorFR));
                motorBLBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorBL));
                motorBRBuffer.Add(new LogPoint(dataSet.TimeStamp, dataSet.MotorSpeeds.MotorBR));
            }
        }

        #endregion
    }
}
