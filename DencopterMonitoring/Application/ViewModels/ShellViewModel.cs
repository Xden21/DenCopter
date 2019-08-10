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

        private IShellService shellService;
        public IShellService ShellService
        {
            get => shellService;
            private set => SetProperty(ref shellService, value);
        }

        #endregion

        #region Constructor

        [ImportingConstructor]
        public ShellViewModel(IShellView view, IShellService shellService)
            : base(view)
        {
            ShellService = shellService;
        }

        #endregion

        #region Methods
               
        public void Show()
        {
            ViewCore.Show();
        }

        public void Close()
        {
            ViewCore.Close();
        }

        #endregion
    }
}
