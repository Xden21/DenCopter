using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.ViewModels;
using DencopterMonitoring.Domain;
using System.ComponentModel.Composition;
using System.Waf.Applications;

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

        private PIDData backupTuning;
        private PIDData incomingTuning;
 
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
            dataService.PIDDataUpdateEvent += IncomingPIDData;
            backupTuning = viewModel.PIDData.Copy();
        }

        #endregion

        #region Methods

        public void Initialize()
        {
            shellService.PID_TuningViewModel = viewModel;
            viewModel.SaveCommand = new DelegateCommand(SaveTuning);
            viewModel.ResetCommand = new DelegateCommand(ResetTuning);
        }

        private void SaveTuning()
        {
            viewModel.PIDData.SetAll();
            generalService.PIDData = viewModel.PIDData.Copy();
            viewModel.DataModified = false;
            backupTuning = viewModel.PIDData;
            incomingTuning = null;
        }

        private void ResetTuning()
        {
            if(incomingTuning != null)
            {
                viewModel.PIDData = incomingTuning.Copy();
                backupTuning = incomingTuning;
                incomingTuning = null;
                viewModel.DataModified = false;
            }
            else
            {
                viewModel.PIDData = backupTuning.Copy();
                viewModel.DataModified = false;

            }
        }

        private void IncomingPIDData(object sender, PIDUpdateEventArgs args)
        {
            if(viewModel.DataModified)
            {
                incomingTuning = args.PIDData;
            }
            else
            {
                viewModel.PIDData = args.PIDData;
                backupTuning = args.PIDData.Copy();
            }
        }
        #endregion
    }
}
