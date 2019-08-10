using System.ComponentModel.Composition;
using System.Waf.Foundation;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(IShellService))]
    public class ShellService: Model, IShellService
    {
        private bool angleVisible;

        public bool AngleVisible
        {
            get { return angleVisible; }
            set { SetProperty(ref angleVisible, value); }
        }
        
        private object angleMonitoringViewModel;

        public object AngleMonitoringViewModel
        {
            get { return angleMonitoringViewModel; }
            set { SetProperty(ref angleMonitoringViewModel, value); }
        }

        private bool motorVisible;

        public bool MotorVisible
        {
            get { return motorVisible; }
            set { SetProperty(ref motorVisible, value); }
        }

        private object motorMonitoringViewModel;

        public object MotorMonitoringViewModel
        {
            get { return motorMonitoringViewModel; }
            set { SetProperty(ref motorMonitoringViewModel, value); }
        }

        private bool pidVisible;

        public bool PIDVisible
        {
            get { return pidVisible; }
            set { SetProperty(ref pidVisible, value); }
        }

        private object pID_TuningViewModel;

        public object PID_TuningViewModel
        {
            get { return pID_TuningViewModel; }
            set { SetProperty(ref pID_TuningViewModel, value); }
        }
        
        private object statusBarViewModel;
        public object StatusBarViewModel
        {
            get { return statusBarViewModel; }
            set { SetProperty(ref statusBarViewModel, value); }
        }        

        public ShellService()
        {
            AngleVisible = true;
            MotorVisible = false;
            PIDVisible = false;
        }
    }
}
