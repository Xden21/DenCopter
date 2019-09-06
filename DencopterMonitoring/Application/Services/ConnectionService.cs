using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(IConnectionService))]
    public class ConnectionService : IConnectionService
    {
        public ConnectionService()
        {
            CurrentConnectionState = ConnectionState.Disconnected;
        }

        public event EventHandler<ConnectionEventArgs> ConnectionChangedEvent;

        public event EventHandler ConnectSwitch;

        public event EventHandler Reset;

        public ConnectionState CurrentConnectionState { get; private set; }

        public void SetConnectionState(ConnectionState connectionState)
        {
            if(CurrentConnectionState != connectionState)
            {
                CurrentConnectionState = connectionState;
                ConnectionChangedEvent?.Invoke(this, new ConnectionEventArgs() { ConnectionState = connectionState });
            }
        }

        public void TriggerConnectSwitch()
        {
            ConnectSwitch?.Invoke(this, EventArgs.Empty);
        }

        public void TriggerReset()
        {
            Reset?.Invoke(this, EventArgs.Empty);
        }
    }

    
    public enum ConnectionState
    {
        Disconnected = 0,
        Connecting = 1,
        Connected = 2
    }

    public class ConnectionEventArgs: EventArgs
    {
        public ConnectionState ConnectionState { get; set; }
    }
}
