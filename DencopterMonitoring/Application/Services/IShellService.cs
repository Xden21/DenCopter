namespace DencopterMonitoring.Application.Services
{
    public interface IShellService
    {
        string CurrentView { get; set; }

        object StatusBarViewModel { get; set; }

        object MonitoringViewModel { get; set; }

        object PID_TuningViewModel { get; set; }

        object CurrentViewModel { get; set; }

        void ChangeView(object arg);
    }
}
