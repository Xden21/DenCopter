using System.ComponentModel.Composition;
using System.Waf.Foundation;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(IShellService))]
    public class ShellService: Model, IShellService
    {

        private string currentView;
        public string CurrentView
        {
            get => currentView;
            set => SetProperty(ref currentView, value);
        }

        private object currentViewModel;
        public object CurrentViewModel
        {
            get => currentViewModel;
            set => SetProperty(ref currentViewModel, value);
        }

        public object MonitoringViewModel { get; set; }

        public object PID_TuningViewModel { get; set; }

        private object statusBarViewModel;
        public object StatusBarViewModel
        {
            get { return statusBarViewModel; }
            set { SetProperty(ref statusBarViewModel, value); }
        }

        public  void ChangeView(object arg)
        {
            string name = arg as string;
            switch (name)
            {
                case "Monitoring":
                    CurrentViewModel = MonitoringViewModel;
                    CurrentView = "Monitoring";
                    break;
                case "PID_Tuning":
                    CurrentViewModel = PID_TuningViewModel;
                    CurrentView = "PID_Tuning";
                    break;
                default:
                    break;
            }
        }
    }
}
