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

        bool Connected
        {
            get;
            set;
        }

        bool Connecting
        {
            get;
            set;
        }

        bool Disconnect
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
