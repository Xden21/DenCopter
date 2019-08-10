using System.ComponentModel;

namespace DencopterMonitoring.Application.Services
{
    public interface IShellService
    {
        event PropertyChangedEventHandler PropertyChanged;
               
        object StatusBarViewModel { get; set; }

        object AngleMonitoringViewModel { get; set; }

        object MotorMonitoringViewModel { get; set; }

        object PID_TuningViewModel { get; set; }

        bool AngleVisible { get; set; }

        bool MotorVisible { get; set; }

        bool PIDVisible { get; set; }
    }
}
