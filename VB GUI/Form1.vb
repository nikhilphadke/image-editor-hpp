Public Class Form1

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load

    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click

        Dim FileToDelete As String

        FileToDelete = "F:\Final Project\Binaries\1.bmp"

        If System.IO.File.Exists(FileToDelete) = True Then

            System.IO.File.Delete(FileToDelete)

        End If


        OpenFileDialog1.Title = "Please Select a File"


        OpenFileDialog1.InitialDirectory = "C:temp"
        OpenFileDialog1.Filter = "BMP files (*.bmp)|*.bmp"

        OpenFileDialog1.ShowDialog()


    End Sub


    Private Sub OpenFileDialog1_FileOk(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles OpenFileDialog1.FileOk

       
        Dim strm As System.IO.Stream

        strm = OpenFileDialog1.OpenFile()


        TextBox1.Text = OpenFileDialog1.FileName.ToString()

        FileCopy(TextBox1.Text, "F:\Final Project\Binaries\1.bmp")
        PictureBox1.Image = Image.FromFile(TextBox1.Text)

    End Sub

    Private Sub TextBox1_TextChanged(sender As Object, e As EventArgs) Handles TextBox1.TextChanged

    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        Process.Start("F:\Final Project\Binaries\Grayscale.exe")
    End Sub

    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
        Process.Start("F:\Final Project\Binaries\Invert.exe")
    End Sub


    Private Sub Button4_Click(sender As Object, e As EventArgs) Handles Button4.Click
        Process.Start("F:\Final Project\Binaries\Cropping.exe")
    End Sub

    Private Sub PictureBox1_Click(sender As Object, e As EventArgs) Handles PictureBox1.Click

    End Sub
End Class
