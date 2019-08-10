using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.ViewModels;
using DencopterMonitoring.Domain;
using LiveCharts.Configurations;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using System.Waf.Applications;

namespace DencopterMonitoring.Application.Controllers
{
    [Export]
    public class ApplicationController
    {
        #region Fields

        private readonly StatusBarController statusBarController;
        private readonly AngleMonitoringController angleMonitoringController;
        private readonly MotorMonitoringController motorMonitoringController;
        private readonly PIDController pIDController;

        private readonly IShellService shellService;
        private readonly ShellViewModel shellViewModel;
        private readonly IGeneralService generalService;
        private readonly ISettingsService settingsService;
        private readonly IDataService dataService;

        private System.Timers.Timer guiUpdateTimer;

        #endregion

        #region Constructor

        [ImportingConstructor]
        public ApplicationController(ISettingsService settingsService,
                                        IGeneralService generalService,
                                        IShellService shellService,
                                        IDataService dataService,
                                        ShellViewModel shellViewModel,
                                        StatusBarController statusBarController,
                                        AngleMonitoringController angleMonitoringController,
                                        MotorMonitoringController motorMonitoringController,
                                        PIDController pIDController)
        {
            this.settingsService = settingsService;
            this.generalService = generalService;
            this.shellService = shellService;
            this.dataService = dataService;
            this.shellViewModel = shellViewModel;
            this.statusBarController = statusBarController;
            this.angleMonitoringController = angleMonitoringController;
            this.motorMonitoringController = motorMonitoringController;
            this.pIDController = pIDController;
        }

        #endregion

        #region Methods

        public void Initialise()
        {
            statusBarController.Initialize();
            angleMonitoringController.Initialize();
            motorMonitoringController.Initialize();
            pIDController.Initialize();
            shellViewModel.ChangeViewCommand = new DelegateCommand(ChangeView);
            SetupCharting();
        }

        public void Run()
        {
            shellViewModel.Show();
        }

        public void Close()
        {

        }

        private void SetupCharting()
        {
            var mapper = Mappers.Xy<LogPoint>()
                .X(value => value.TimeVal)
                .Y(value => value.Value);
            LiveCharts.Charting.For<LogPoint>(mapper);
        }

        public void StartGuiUpdate()
        {
            guiUpdateTimer = new System.Timers.Timer()
            {
                Interval = settingsService.FPS,
                AutoReset = true
            };
            guiUpdateTimer.Elapsed += UpdateData;
            guiUpdateTimer.Start();
        }

        public void StopGuiUpdate()
        {
            if (guiUpdateTimer != null)
            {
                guiUpdateTimer.Stop();
                guiUpdateTimer.Dispose();
                guiUpdateTimer = null;
            }
        }

        private void UpdateData(object sender, ElapsedEventArgs args)
        {
            if (generalService.Connected)
            {
                List<DataSet> newDataSets;
                lock (dataService.DataLock)
                {
                    // Load unprocessed datasets
                    newDataSets = dataService.UnprocessedDataSets;
                    dataService.UnprocessedDataSets = new List<DataSet>();
                }
                // Add data to backup buffer.
                if (newDataSets != null && newDataSets.Count != 0)
                {
                    lock (dataService.AllDataSets)
                    {
                        dataService.AllDataSets.AddRange(newDataSets);
                    }

                    // Trigger update
                    dataService.TriggerDataUpdateEvent(newDataSets);
                }
            }
        }

        public void ChangeView(object arg)
        {
            string name = arg as string;
            StopGuiUpdate();
            Thread.Sleep(50);
            switch (name)
            {
                case "AngleMonitoring":
                    shellService.AngleVisible = true;
                    shellService.MotorVisible = false;
                    shellService.PIDVisible = false;
                    break;
                case "MotorMonitoring":
                    shellService.AngleVisible = false;
                    shellService.MotorVisible = true;
                    shellService.PIDVisible = false;
                    break;
                case "PID_Tuning":
                    shellService.AngleVisible = false;
                    shellService.MotorVisible = false;
                    shellService.PIDVisible = true;
                    break;
                default:
                    shellService.AngleVisible = false;
                    shellService.MotorVisible = false;
                    shellService.PIDVisible = false;
                    break;
            }
            StartGuiUpdate();
        }

        #endregion

    }
}
