using DencopterMonitoring.Application.Views;
using System;
using System.ComponentModel;
using System.ComponentModel.Composition;
using System.Waf.Applications;
using System.Windows.Input;

namespace DencopterMonitoring.Application.ViewModels
{
    [Export]
    public class StatusBarViewModel : ViewModel<IStatusBarView>
    {
        [ImportingConstructor]
        public StatusBarViewModel(IStatusBarView view) : base(view)
        {
            connectionState = "Disconnected";
            _ARMStatus = "Disarmed";
            flightMode = "Rate Mode";
        }

        private bool connected;

        public bool Connected
        {
            get { return connected; }
            set { SetProperty(ref connected, value); }
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

        private ICommand disconnectCommand;

        public ICommand ConnectionSwitchCommand
        {
            get { return disconnectCommand; }
            set { SetProperty(ref disconnectCommand, value); }
        }

        private ICommand resetCommand;

        public ICommand ResetCommand
        {
            get { return resetCommand; }
            set { SetProperty(ref resetCommand, value); }
        }

    }
}
