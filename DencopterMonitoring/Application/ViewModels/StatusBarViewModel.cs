using DencopterMonitoring.Application.Views;
using System.Waf.Applications;

namespace DencopterMonitoring.Application.ViewModels
{
    public class StatusBarViewModel : ViewModel<IStatusBarView>
    {
        public StatusBarViewModel(IStatusBarView view) : base(view)
        {
            connectionState = "Disconnected";
            _ARMStatus = "Disarmed";
            flightMode = "Landed";
        }

        private string connectionState;

        public string ConnectionState
        {
            get { return connectionState; }
            set { SetProperty(ref connectionState, value); }
        }

        private string _ARMStatus;

        public string ARMStatus
        {
            get { return _ARMStatus; }
            set { SetProperty(ref _ARMStatus, value); }
        }

        private string flightMode;

        public string FlightMode
        {
            get { return flightMode; }
            set { SetProperty(ref flightMode, value); }
        }
    }
}
