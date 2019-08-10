namespace DencopterMonitoring.Application.Services
{
    public interface ISettingsService
    {
        double TimeFrame { get; }

        string SaveFolder { get; }

        int FPS { get; }

        string InetAddress { get; }

        int Port { get; }

        int LoggerFreq { get; }
    }
}
