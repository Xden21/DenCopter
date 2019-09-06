using DencopterMonitoring.Application.Views;
using DencopterMonitoring.Domain;
using System;
using System.ComponentModel;
using System.ComponentModel.Composition;
using System.Waf.Applications;
using System.Windows.Input;

namespace DencopterMonitoring.Application.ViewModels
{
    [Export]
    public class PIDControlViewModel : ViewModel<IPIDControlView>
    {
        [ImportingConstructor]
        public PIDControlViewModel(IPIDControlView view) : base(view)
        {
            dataModified = false;
            PIDData = new PIDData();
        }

        private PIDData pIDData;

        public PIDData PIDData
        {
            get { return pIDData; }
            set
            {
                if(pIDData != null)
                    pIDData.DataChanged -= PIDDataUpdated;
                SetProperty(ref pIDData, value);
                pIDData.DataChanged += PIDDataUpdated;
            }
        }

        private bool dataModified;

        public bool DataModified
        {
            get { return dataModified; }
            set { SetProperty(ref dataModified, value); }
        }

        private ICommand saveCommand;

        public ICommand SaveCommand
        {
            get { return saveCommand; }
            set { SetProperty(ref saveCommand, value); }
        }

        private ICommand resetCommand;

        public ICommand ResetCommand
        {
            get { return resetCommand; }
            set { SetProperty(ref resetCommand, value); }
        }
        
        private void PIDDataUpdated(object sender, EventArgs args)
        {
            DataModified = PIDData.IsChanged();
        }
    }
}
