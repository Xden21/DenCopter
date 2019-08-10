using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.ViewModels;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Controllers
{
    [Export]
    public class StatusBarController
    {
        #region Fields

        private readonly IGeneralService generalService;
        private readonly StatusBarViewModel statusBarViewModel;
        private readonly IShellService shellService;

        #endregion

        #region Constructor

        [ImportingConstructor]
        public StatusBarController(StatusBarViewModel statusBarViewModel ,IGeneralService generalService, IShellService shellService)
        {
            this.statusBarViewModel = statusBarViewModel;
            this.generalService = generalService;
            this.shellService = shellService;
            generalService.PropertyChanged += connectionUpdated;
        }

        #endregion

        #region Methods

        public void Initialize()
        {
            shellService.StatusBarViewModel = statusBarViewModel;
        }
               
        private void connectionUpdated(object sender, PropertyChangedEventArgs args)
        {
            if (args.PropertyName == "Connected")
            {
                if (generalService.Connected)
                {
                    statusBarViewModel.ConnectionState = "Connected";
                }
                else if (generalService.Connecting)
                {
                    statusBarViewModel.ConnectionState = "Connecting";
                }
                else
                {
                    statusBarViewModel.ConnectionState = "Disconnected";
                }
            }
            if (args.PropertyName == "Connecting")
            {
                if (generalService.Connecting)
                {
                    statusBarViewModel.ConnectionState = "Connected";
                }
                else if (generalService.Connected)
                {
                    statusBarViewModel.ConnectionState = "Connecting";
                }
                else
                {
                    statusBarViewModel.ConnectionState = "Disconnected";
                }
            }
        }

        #endregion
    }
}
