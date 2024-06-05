using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WPF_GradedLab
{
    public enum Currency { PLN, USD, EUR, GBP, NOK }
    public enum Role { Worker, SeniorWorker, IT, Manager, Director, CEO }
    public class Employee : INotifyPropertyChanged
    {

        private string firstName;
        public string FirstName
        {
            get { return firstName; }
            set
            {
                firstName = value;
                OnPropertyChanged(nameof(FirstName));
            }
        }
        public string lastName { get; set; }
        public string LastName
        {
            get { return lastName; }
            set
            {
                lastName = value;
                OnPropertyChanged(nameof(lastName));
            }
        }
        public string sex { get; set; }
        public string Sex
        {
            get { return sex; }
            set
            {
                sex = value;
                OnPropertyChanged(nameof(sex));
            }
        }
        public DateTime birthDate { get; set; }
        public DateTime BirthDate
        {
            get { return birthDate; }
            set
            {
                birthDate = value;
                OnPropertyChanged(nameof(birthDate));
            }
        }
        public string birthCountry { get; set; }
        public string BirthCountry
        {
            get { return birthCountry; }
            set
            {
                birthCountry = value;
                OnPropertyChanged(nameof(birthCountry));
            }
        }
        public int salary { get; set; }
        public int Salary
        {
            get { return salary; }
            set
            {
                salary = value;
                OnPropertyChanged(nameof(salary));
            }
        }
        public Currency salaryCurrency { get; set; }
        public Currency SalaryCurrency
        {
            get { return salaryCurrency; }
            set
            {
                salaryCurrency = value;
                OnPropertyChanged(nameof(salaryCurrency));
            }
            
        }
        public Role companyRole { get; set; }
        public Role CompanyRole
        {
            get { return companyRole; }
            set
            {
                companyRole = value;
                OnPropertyChanged(nameof(companyRole));
            }
        }

        public Employee() { }
        public Employee(string firstName, string lastName, string sex, DateTime birthDate, string birthCountry, int salary, Currency salaryCurrency, Role companyRole)
        {
            FirstName = firstName;
            LastName = lastName;
            Sex = sex;
            BirthDate = birthDate;
            BirthCountry = birthCountry;
            Salary = salary;
            SalaryCurrency = salaryCurrency;
            CompanyRole = companyRole;
        }


        public event PropertyChangedEventHandler PropertyChanged;

        public bool hasChangesData = false;

        public bool HasChanged
        {
            get { return hasChangesData; }
            set
            {
                hasChangesData = value;
                OnPropertyChanged(nameof(hasChangesData));
            }
        }

        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
