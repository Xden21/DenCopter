using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.ViewModels;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Applications;

namespace DencopterMonitoring.Application.Controllers
{
    [Export]
    public class StatusBarController
    {
        #region Fields

        private readonly IGeneralService generalService;
        private readonly StatusBarViewModel statusBarViewModel;
        private readonly IShellService shellService;
        private readonly IConnectionService connectionService;

        #endregion

        #region Constructor

        [ImportingConstructor]
        public StatusBarController(StatusBarViewModel statusBarViewModel, IGeneralService generalService, IShellService shellService, IConnectionService connectionService)
        {
            this.statusBarViewModel = statusBarViewModel;
            this.generalService = generalService;
            this.shellService = shellService;
            this.connectionService = connectionService;
            connectionService.ConnectionChangedEvent += ConnectionUpdated;
            generalService.PropertyChanged += GeneralServicePropertyChanged;
        }

        #endregion

        #region Methods

        public void Initialize()
        {
            shellService.StatusBarViewModel = statusBarViewModel;
            statusBarViewModel.ConnectionSwitchCommand = new DelegateCommand(ConnectionSwitchCommand);
            statusBarViewModel.ResetCommand = new DelegateCommand(ResetCommand);
        }

        private void ConnectionSwitchCommand()
        {
            connectionService.TriggerConnectSwitch();
        }

        private void ResetCommand()
        {
            connectionService.TriggerReset();
        }

        private void ConnectionUpdated(object sender, ConnectionEventArgs args)
        {
            if (args.ConnectionState == ConnectionState.Connected)
            {
                statusBarViewModel.ConnectionState = "Connected";
                statusBarViewModel.Connected = true;
            }
            else if (args.ConnectionState == ConnectionState.Connecting)
            {
                statusBarViewModel.ConnectionState = "Connecting";
                statusBarViewModel.Connected = false;

            }
            else
            {
                statusBarViewModel.ConnectionState = "Disconnected";
                statusBarViewModel.Connected = false;
            }
        }

        private void GeneralServicePropertyChanged(object sender, PropertyChangedEventArgs args)
        {
            if (args.PropertyName == "FlightMode")
            {
                switch (generalService.FlightMode)
                {
                    case 0:
                        statusBarViewModel.FlightMode = "Rate Mode";
                        break;
                    case 1:
                        statusBarViewModel.FlightMode = "Angle Mode";
                        break;
                    default:
                        statusBarViewModel.FlightMode = "UNKNOWN";
                        break;
                }
            }
            else if (args.PropertyName == "Armed")
            {
                switch (generalService.Armed)
                {
                    case 0:
                        statusBarViewModel.ARMStatus = "Disarmed";
                        break;
                    case 1:
                        statusBarViewModel.ARMStatus = "Arming";
                        break;
                    case 2:
                        statusBarViewModel.ARMStatus = "ARMED";
                        break;
                    default:
                        statusBarViewModel.ARMStatus = "UNKNOWN";
                        break;
                }
            }
        }



        #endregion
    }
}
