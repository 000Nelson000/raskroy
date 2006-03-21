﻿namespace Denisenko.Cutting.CutOptima
{
	partial class ProgressForm
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
			this.components = new System.ComponentModel.Container();
			this.progressBar = new System.Windows.Forms.ProgressBar();
			this.cancelButton = new System.Windows.Forms.Button();
			this.pauseResumeButton = new System.Windows.Forms.Button();
			this.progressTimer = new System.Windows.Forms.Timer(this.components);
			this.SuspendLayout();
			// 
			// progressBar
			// 
			this.progressBar.Location = new System.Drawing.Point(12, 12);
			this.progressBar.Name = "progressBar";
			this.progressBar.Size = new System.Drawing.Size(291, 23);
			this.progressBar.TabIndex = 0;
			// 
			// cancelButton
			// 
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(174, 82);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(75, 23);
			this.cancelButton.TabIndex = 1;
			this.cancelButton.Text = "Отменить";
			this.cancelButton.UseVisualStyleBackColor = true;
			this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
			// 
			// pauseResumeButton
			// 
			this.pauseResumeButton.Enabled = false;
			this.pauseResumeButton.Location = new System.Drawing.Point(65, 82);
			this.pauseResumeButton.Name = "pauseResumeButton";
			this.pauseResumeButton.Size = new System.Drawing.Size(75, 23);
			this.pauseResumeButton.TabIndex = 2;
			this.pauseResumeButton.Text = "Пауза";
			this.pauseResumeButton.UseVisualStyleBackColor = true;
			this.pauseResumeButton.Click += new System.EventHandler(this.pauseResumeButton_Click);
			// 
			// progressTimer
			// 
			this.progressTimer.Tick += new System.EventHandler(this.progressTimer_Tick);
			// 
			// ProgressForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(315, 130);
			this.Controls.Add(this.pauseResumeButton);
			this.Controls.Add(this.cancelButton);
			this.Controls.Add(this.progressBar);
			this.Name = "ProgressForm";
			this.Text = "Выполняется оптимизация";
			this.Load += new System.EventHandler(this.ProgressForm_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ProgressBar progressBar;
		private System.Windows.Forms.Button cancelButton;
		private System.Windows.Forms.Button pauseResumeButton;
		private System.Windows.Forms.Timer progressTimer;
	}
}