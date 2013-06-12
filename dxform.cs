class Form1 : Form 
{	

	private Margins marg;

	//this is used to specify the boundaries of the transparent area
	internal struct Margins
	{
		public int Left, Right, Top, Bottom;
	}

	[DllImport("user32.dll", SetLastError = true)]

	private static extern UInt32 GetWindowLong(IntPtr hWnd, int nIndex);

	[DllImport("user32.dll")]

	static extern int SetWindowLong(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

	[DllImport("user32.dll")]

	static extern bool SetLayeredWindowAttributes(IntPtr hwnd, uint crKey, byte bAlpha, uint dwFlags);

	public const int GWL_EXSTYLE = -20;

	public const int WS_EX_LAYERED = 0x80000;

	public const int WS_EX_TRANSPARENT = 0x20;

	public const int LWA_ALPHA = 0x2;

	public const int LWA_COLORKEY = 0x1;

	[DllImport("dwmapi.dll")]
	static extern void DwmExtendFrameIntoClientArea(IntPtr hWnd, ref Margins pMargins);

	private Device device = null; 
	
	
	
	public Form1() 
	{
		//Make the window's border completely transparant
		SetWindowLong(this.Handle, GWL_EXSTYLE,
				(IntPtr)(GetWindowLong(this.Handle, GWL_EXSTYLE) ^ WS_EX_LAYERED ^ WS_EX_TRANSPARENT));

		//Set the Alpha on the Whole Window to 255 (solid)
		SetLayeredWindowAttributes(this.Handle, 0, 255, LWA_ALPHA);

		//Init DirectX
				//This initializes the DirectX device. It needs to be done once.
		//The alpha channel in the backbuffer is critical.
		PresentParameters presentParameters = new  PresentParameters();
		presentParameters.Windowed = true;
		presentParameters.SwapEffect = SwapEffect.Discard;
		presentParameters.BackBufferFormat = Format.A8R8G8B8;
		
		this.BackColor = System.Drawing.Color.Black;
		this.FormBorderStyle = FormBorderStyle.None;
		this.ShowIcon = false;
		this.ShowInTaskbar = false;
		this.TopMost = true;
		this.WindowState = FormWindowState.Maximized;

		this.device = new Device(0, DeviceType.Hardware,  this.Handle,
		CreateFlags.HardwareVertexProcessing, presentParameters);


		Thread dx = new Thread(new ThreadStart(this.dxThread));
		dx.IsBackground = true;
		dx.Start();
	}
	
	private void dxThread()
	{
		while (true)
		{
			//Place your update logic here
			device.Clear(ClearFlags.Target, Color.FromArgb(0, 0, 0, 0), 1.0f, 0);
			device.RenderState.ZBufferEnable = false;
			device.RenderState.Lighting = false;
			device.RenderState.CullMode = Cull.None;
			device.Transform.Projection = Matrix.OrthoOffCenterLH(0, this.Width, this.Height, 0, 0, 1);
			device.BeginScene();

			//Place your rendering logic here

			device.EndScene();
			device.Present();
		}

		this.device.Dispose();
		Application.Exit();
	}
	
	private void onPaint()
	{
			//Create a margin (the whole form)
		marg.Left = 0;
		marg.Top = 0;
		marg.Right = this.Width;
		marg.Bottom = this.Height;

		//Expand the Aero Glass Effect Border to the WHOLE form.
		// since we have already had the border invisible we now
		// have a completely invisible window - apart from the DirectX
		// renders NOT in black.
		DwmExtendFrameIntoClientArea(this.Handle, ref marg);
	}
}