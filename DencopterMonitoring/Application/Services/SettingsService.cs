using System.ComponentModel.Composition;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(ISettingsService))]
    public class SettingsService: ISettingsService
    {
        #region Properties

        public double TimeFrame { get; }
        public string SaveFolder { get; }

        public int FPS { get; }

        public string InetAddress { get; }

        public int Port { get; }

        public int LoggerFreq { get; }

        #endregion

        #region Constructor

        public SettingsService()
        {
            TimeFrame = Properties.Settings.Default.TimeFrame;
            SaveFolder = Properties.Settings.Default.SaveFolder;
            FPS = Properties.Settings.Default.FPS;
            InetAddress = Properties.Settings.Default.InetAddress;
            Port = Properties.Settings.Default.Port;
            LoggerFreq = Properties.Settings.Default.LoggerFrequency;
        }

        #endregion
    }
}
