namespace TibiaDatReader
{
    partial class TibiaDataMainDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.LoadDatFile = new System.Windows.Forms.Button();
            this.debugInfo = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // LoadDatFile
            // 
            this.LoadDatFile.Location = new System.Drawing.Point(371, 297);
            this.LoadDatFile.Name = "LoadDatFile";
            this.LoadDatFile.Size = new System.Drawing.Size(75, 23);
            this.LoadDatFile.TabIndex = 0;
            this.LoadDatFile.Text = "Load dat file";
            this.LoadDatFile.UseVisualStyleBackColor = true;
            this.LoadDatFile.Click += new System.EventHandler(this.LoadDatFile_Click);
            // 
            // debugInfo
            // 
            this.debugInfo.FormattingEnabled = true;
            this.debugInfo.Location = new System.Drawing.Point(12, 13);
            this.debugInfo.Name = "debugInfo";
            this.debugInfo.Size = new System.Drawing.Size(739, 277);
            this.debugInfo.TabIndex = 1;
            // 
            // TibiaDataMainDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(763, 332);
            this.Controls.Add(this.debugInfo);
            this.Controls.Add(this.LoadDatFile);
            this.Name = "TibiaDataMainDialog";
            this.Text = "Tibia Dat Reader";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button LoadDatFile;
        private System.Windows.Forms.ListBox debugInfo;
    }
}

