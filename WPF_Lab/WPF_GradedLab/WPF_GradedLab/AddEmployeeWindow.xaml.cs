using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace WPF_GradedLab
{
    /// <summary>
    /// Interaction logic for AddEmployeeWindow.xaml
    /// </summary>
    public partial class AddEmployeeWindow : Window
    {
        public Employee NewEmployee { get; private set; }

        public AddEmployeeWindow()
        {
            InitializeComponent();
            NewEmployee = new Employee();
            this.DataContext = new EmployeeViewModel();
        }

        private void Add_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
            if (Owner is MainWindow mainWindow)
            {
                mainWindow.Employees.Add(NewEmployee);
                mainWindow.hasChanges = true;
            }
            Close();
        }
    }
}
