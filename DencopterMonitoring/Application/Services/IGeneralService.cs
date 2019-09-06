using DencopterMonitoring.Domain;
using System.ComponentModel;

namespace DencopterMonitoring.Application.Services
{
    public interface IGeneralService
    {
        event PropertyChangedEventHandler PropertyChanged;

        int FlightMode
        {
            get;
            set;
        }

        int Armed
        {
            get;
            set;
        }

        bool NewPIDData
        {
            get;
        }

        PIDData PIDData
        {
            get;
            set;
        }
    }
}
