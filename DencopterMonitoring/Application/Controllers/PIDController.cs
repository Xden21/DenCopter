using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.ViewModels;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Controllers
{
    [Export]
    public class PIDController
    {
        #region Fields

        private readonly PIDControlViewModel viewModel;

        private readonly IShellService shellService;
        private readonly ISettingsService settingsService;
        private readonly IGeneralService generalService;
        private readonly IDataService dataService;

        #endregion

        #region Constructor

        [ImportingConstructor]
        public PIDController(   PIDControlViewModel viewModel,
                                IShellService shellService,
                                ISettingsService settingsService,
                                IGeneralService generalService,
                                IDataService dataService)
        {
            this.viewModel = viewModel;
            this.shellService = shellService;
            this.settingsService = settingsService;
            this.generalService = generalService;
            this.dataService = dataService;
        }

        #endregion

        #region Methods

        public void Initialize()
        {
            shellService.PID_TuningViewModel = viewModel;
        }

        #endregion
    }
}
