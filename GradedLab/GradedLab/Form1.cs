namespace GradedLab
{
    public partial class Form1 : Form
    {
        private bool isDrawing = false;
        private Point prevPoint;
        //private toolStripStatusLabel1 mouseposition;
        public Form1()
        {
            InitializeComponent();
            //toolStripStatusLabel1.MouseMove += ToolStrip_MouseMove;
            pictureBox1.MouseDown += PictureBox1_MouseDown;
            pictureBox1.MouseMove += PictureBox1_MouseMove;
            pictureBox1.MouseUp += PictureBox1_MouseUp;
        }

        /*
        private void ToolStrip_MouseMove(object sender, MouseEventArgs e)
        {
            mousePosition.Text = $"{e.X}, {e.Y} px";
        }
        */

        private void PictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            isDrawing = true;
            prevPoint = e.Location;
        }

        private void PictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (isDrawing)
            {
                using (Graphics g = pictureBox1.CreateGraphics())
                {
                    Pen pen = new Pen(Color.Black, 1);
                    g.DrawLine(pen, prevPoint, e.Location);
                    prevPoint = e.Location;
                }
            }
        }
        private void PictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            isDrawing = false;
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Image Files|*bitmap;*.jpeg;*.png;*";
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    pictureBox1.Image = Image.FromFile(openFileDialog.FileName);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error loading an image");
                }
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {

            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "JPEG Image|*.jpg*";
            saveFileDialog.Title = "Image";
            saveFileDialog.ShowDialog();

            if(saveFileDialog.FileName != "")
            {
                try
                {
                    pictureBox1.Image.Save(saveFileDialog.FileName);
                    MessageBox.Show("Image saved");
                }
                catch(Exception ex)
                {
                    MessageBox.Show("Error while saving");
                }
            }
        }
    }
}
