using System.ComponentModel.Composition;

namespace DencopterMonitoring.Application.Services
{
    [Export(typeof(ISettingsService))]
    public class SettingsService: ISettingsService
    {
    }
}
