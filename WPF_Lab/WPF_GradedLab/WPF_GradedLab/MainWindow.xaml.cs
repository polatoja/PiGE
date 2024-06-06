using Microsoft.Win32;
using System.Collections.ObjectModel;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.ComponentModel;

namespace WPF_GradedLab
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public ObservableCollection<Employee> Employees { get; set; } = new ObservableCollection<Employee>();
        public AddEmployeeWindow addEmployeeWindow;
        private string lastFilePath = "";
        public bool hasChanges = false;

        public MainWindow()
        {
            InitializeComponent();
            DataContext = this;
            addEmployeeWindow  = new AddEmployeeWindow();
        }
        private void OpenFile_Click(object sender, RoutedEventArgs e)
        {
            if (hasChanges)
            {
                MessageBoxResult result = MessageBox.Show("Do you want to save changes?", "Save Changes", MessageBoxButton.YesNoCancel);
                switch (result)
                {
                    case MessageBoxResult.Yes:
                        Save_Click(null, null);
                        break;
                    case MessageBoxResult.No:
                        break;
                    default:
                        break;
                }
            }

            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "CSV files (*.csv)|*.csv";
            if (openFileDialog.ShowDialog() == true)
            {
                Employees.Clear();
                lastFilePath = openFileDialog.FileName;

                string[] lines = File.ReadAllLines(openFileDialog.FileName);

                for (int i = 1; i < lines.Length; i++)
                {
                    string line = lines[i];
                    string[] fields = line.Split(';');

                    if (fields.Length >= 8)
                    {
                        string firstName = fields[0];
                        string lastName = fields[1];
                        string sex = fields[2];
                        DateTime birthDate = DateTime.Parse(fields[3]);
                        string birthCountry = fields[4];
                        int salary = int.Parse(fields[5]);
                        Currency salaryCurrency = (Currency)Enum.Parse(typeof(Currency), fields[6]);
                        Role companyRole = (Role)Enum.Parse(typeof(Role), fields[7]);

                        Employees.Add(new Employee(firstName, lastName, sex, birthDate, birthCountry, salary, salaryCurrency, companyRole));
                    }
                }
            }
        }


        private void SaveFile_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "CSV files (*.csv)|*.csv";
            if (saveFileDialog.ShowDialog() == true)
            {
                lastFilePath = saveFileDialog.FileName;
                SaveDataToFile(lastFilePath);
            }
            ResetChangesFlag();
        }

        private void Save_Click(object sender, RoutedEventArgs e)
        {
            SaveDataToFile(lastFilePath);
            ResetChangesFlag();
        }

        private void Close_Click(object sender, RoutedEventArgs e)
        {
            if (hasChanges)
            {
                MessageBoxResult result = MessageBox.Show("Do you want to save changes?", "Save Changes", MessageBoxButton.YesNoCancel);
                switch (result)
                {
                    case MessageBoxResult.Yes:
                        SaveFile_Click(null, null);
                        if (!hasChanges)
                        {
                            Close();
                        }
                        break;
                    case MessageBoxResult.No:
                        Close();
                        break;
                    default:
                        break;
                }
            }
            else
            {
                Close();
            }
        }

        private void ResetChangesFlag()
        {
            hasChanges = false;
            foreach (var employee in Employees)
            {
                employee.HasChanged = false;
            }
        }
        private void SaveDataToFile(string filePath)
        { 
            if(!string.IsNullOrEmpty(filePath))
            {
                using (StreamWriter writer = new StreamWriter(filePath))
                {
                    writer.WriteLine("FirstName;LastName;Sex;BirthDate;BirthCountry;Salary;SalaryCurrency;CompanyRole");
                    foreach (var employee in Employees)
                    {
                        writer.WriteLine($"{employee.FirstName};{employee.LastName};{employee.Sex};{employee.BirthDate:dd.MM.yyyy};{employee.BirthCountry};{employee.Salary};{(int)employee.SalaryCurrency};{(int)employee.CompanyRole}");
                    }
                }
            }
        }

        private void ComboBox_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            if (IsLoaded && e.RemovedItems.Count != 0)
            {
                hasChanges = true;
            }
        }
        private void TextBox_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            if (IsLoaded)
            {
                hasChanges = true;
            }
        }

        private int GetSelectedIndex()
        {
            var listBox = EmployeesListBox;
            if (listBox != null && listBox.SelectedItem != null)
            {
                return listBox.SelectedIndex;
            }
            return -1;
        }

        private void MoveUp_Click(object sender, RoutedEventArgs e)
        {
            var selectedIndex = GetSelectedIndex();
            if (selectedIndex > 0)
            {
                var itemToMoveUp = Employees[selectedIndex];
                Employees.RemoveAt(selectedIndex);
                Employees.Insert(selectedIndex - 1, itemToMoveUp);
                hasChanges = true;
            }
        }

        private void MoveDown_Click(object sender, RoutedEventArgs e)
        {
            var selectedIndex = GetSelectedIndex();
            if (selectedIndex < Employees.Count - 1 && selectedIndex >= 0)
            {
                var itemToMoveDown = Employees[selectedIndex];
                Employees.RemoveAt(selectedIndex);
                Employees.Insert(selectedIndex + 1, itemToMoveDown);
                hasChanges = true;
            }
        }

        private void AddEmployee_Click(object sender, RoutedEventArgs e)
        {
            var addEmployeeWindow = new AddEmployeeWindow();
            addEmployeeWindow.Owner = this;
            addEmployeeWindow.ShowDialog();
        }


        private void DeleteEmployee_Click(object sender, RoutedEventArgs e)
        {
            var selectedIndex = GetSelectedIndex();
            Employees.RemoveAt(selectedIndex);
        }
    }
}