using DencopterMonitoring.Application.Services;
using DencopterMonitoring.Application.Views;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Applications;

namespace DencopterMonitoring.Application.ViewModels
{
    [Export]
    public class ShellViewModel: ViewModel<IShellView>
    {
        #region Fields


        private DelegateCommand changeViewCommand;

        #endregion

        #region Properties

        public DelegateCommand ChangeViewCommand
        {
            get => changeViewCommand;
            set => SetProperty(ref changeViewCommand, value);
        }

        public IShellService ShellService
        {
            get;
        }

        #endregion

        #region Constructor

        [ImportingConstructor]
        public ShellViewModel(IShellView view, IShellService shellService)
            : base(view)
        {
            ShellService = shellService;
            ChangeViewCommand = new DelegateCommand(ShellService.ChangeView);
        }

        #endregion

        #region Methods
               

        #endregion
    }
}
