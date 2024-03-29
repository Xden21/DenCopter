﻿using DencopterMonitoring.Application.Controllers;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.ComponentModel.Composition.Hosting;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using System.Waf.Applications.Services;
using System.Windows;
using System.Windows.Navigation;

namespace DencopterMonitoring
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        AggregateCatalog catalog;
        CompositionContainer container;
        ApplicationController applicationController;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            catalog = new AggregateCatalog();
            catalog.Catalogs.Add(new AssemblyCatalog(typeof(ApplicationController).Assembly));
            container = new CompositionContainer(catalog);
            applicationController = container.GetExportedValue<ApplicationController>();
            applicationController.Initialise();
            applicationController.Run();
        }

        protected override void OnExit(ExitEventArgs e)
        {
            applicationController.Close();

            base.OnExit(e);
        }
    }
}
