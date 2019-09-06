using DencopterMonitoring.Domain;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Waf.Foundation;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(IGeneralService))]
    public class GeneralService: Model, IGeneralService
    {
        public GeneralService()
        {
            flightMode = 0;
        }

        private int flightMode;

        public int FlightMode
        {
            get { return flightMode; }
            set { SetProperty(ref flightMode, value); }
        }

        private int armed;

        public int Armed
        {
            get { return armed; }
            set { SetProperty(ref armed, value); }
        }


        private PIDData pidData;

        public PIDData PIDData
        {
            get
            {
                NewPIDData = false;
                return pidData;
            }
            set
            {
                SetProperty(ref pidData, value);
                NewPIDData = true;
            }
        }


        private bool newPIDData;

        public bool NewPIDData
        {
            get { return newPIDData; }
            private set { SetProperty(ref newPIDData, value); }
        }

    }
}
