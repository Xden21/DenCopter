using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Services
{
    public interface IConnectionService
    {
        event EventHandler<ConnectionEventArgs> ConnectionChangedEvent;

        event EventHandler ConnectSwitch;

        event EventHandler Reset;

        void SetConnectionState(ConnectionState connectionState);

        void TriggerConnectSwitch();

        void TriggerReset();

        ConnectionState CurrentConnectionState { get; }
    }
}
