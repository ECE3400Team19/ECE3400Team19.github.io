`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144

///////* DON'T CHANGE THIS PART *///////
module DE0_NANO(
	CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY
);

//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED = 8'b111_000_00;
localparam GREEN = 8'b000_111_00;
localparam BLUE = 8'b000_000_11;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK - DON'T NEED TO CHANGE THIS //////////
input 		          		CLOCK_50;

//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:20]		GPIO_1_D;
input 		     [1:0]		KEY;

// input clock signals from camera
wire PCLK ;
wire HREF ;
wire V_SYNC ;


///// PIXEL DATA /////
reg toggle ;
reg toggle_en ;

///// READ/WRITE ADDRESS /////
reg [14:0] X_ADDR;
reg [14:0] Y_ADDR;
wire [14:0] WRITE_ADDRESS;
reg [14:0] READ_ADDRESS; 

assign PCLK = GPIO_1_D[28];
assign HREF = GPIO_1_D[29];
assign V_SYNC = GPIO_1_D[30];

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);
//assign WRITE_ADDRESS = VGA_PIXEL_X + (VGA_PIXEL_Y)*(`SCREEN_WIDTH);

assign GPIO_0_D[31:24] = RESULT;

///// VGA INPUTS/OUTPUTS /////
wire 			VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire			VGA_VSYNC_NEG;
wire			VGA_HSYNC_NEG;
reg			VGA_READ_MEM_EN;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
wire [7:0] RESULT;
/* WRITE ENABLE */
reg W_EN;

///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////
wire c0_sig, c1_sig, c2_sig;
assign GPIO_0_D[33] = c0_sig;

///////* INSTANTIATE YOUR PLL HERE *///////
PLL_Nopls	PLL_Nopls_inst (
	.inclk0 ( CLOCK_50 ),
	.c0 ( c0_sig ),
	.c1 ( c1_sig ),
	.c2 ( c2_sig )
	);



///////* M9K Module *///////
Dual_Port_RAM_M9K mem(
	.input_data(pixel_data_RGB332),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W(CLOCK_50),
	.clk_R(c1_sig), // DO WE NEED TO READ SLOWER THAN WRITE??
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(c1_sig),
	.PIXEL_COLOR_IN(VGA_READ_MEM_EN ? MEM_OUTPUT : BLUE),
	.PIXEL_X(VGA_PIXEL_X),
	.PIXEL_Y(VGA_PIXEL_Y),
	.PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
   .H_SYNC_NEG(GPIO_0_D[7]),
   .V_SYNC_NEG(VGA_VSYNC_NEG)
);

///////* Image Processor *///////
IMAGE_PROCESSOR proc(
	.PIXEL_IN(MEM_OUTPUT),
	.CLK(c1_sig),
	.VGA_PIXEL_X(VGA_PIXEL_X),
	.VGA_PIXEL_Y(VGA_PIXEL_Y),
	.VGA_VSYNC_NEG(VGA_VSYNC_NEG),
	.RESULT(RESULT) 
);

localparam IDLE = 3'b0;
localparam BYTE1 = 3'b01;
localparam BYTE2 = 3'b10; 
localparam RESET = 3'b11;
localparam LINE_OVER = 3'b100;
reg[2:0] state;
reg[2:0] next_state;
reg [7:0]	pixel_data_RGB332;
reg [7:0]   signal_part_1 = 8'd0;
reg [7:0]   signal_part_1_out = 8'd0;
reg signal_part_1_en;
reg x_inc_en;
reg y_inc_en;
reg y_reset;
reg x_reset;

///////* Update Read Address *///////
always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
		//read memory
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
				VGA_READ_MEM_EN = 1'b0;
		end
		else begin
				VGA_READ_MEM_EN = 1'b1;
		end
end

//state transitions

always @ (*) begin
	case (state) 
		IDLE: begin
		if (V_SYNC) next_state = RESET;
		else if (HREF) next_state = BYTE1;
		else next_state = IDLE;
		end
		BYTE1: begin
		if (V_SYNC) next_state = RESET;
		else if (HREF) next_state = BYTE2;
		else next_state = LINE_OVER;
		end
		BYTE2: begin
		if (V_SYNC) next_state = RESET;
		else if (HREF) next_state = BYTE1;
		else next_state = LINE_OVER;
		end
		RESET: begin 
		if (V_SYNC) next_state = RESET;
		else next_state = IDLE;
		end
		LINE_OVER: begin
			if (V_SYNC) next_state = RESET;
			else next_state = IDLE;
		end
		default: next_state = RESET;
		
	endcase

end

//output
always @ (*) begin
	case (state)
		IDLE: begin
			pixel_data_RGB332 = 8'b0;
			signal_part_1 = 8'd0;
			signal_part_1_en = 1'b0; 
			W_EN = 0;
			//x and y
			x_reset = 0;
			y_reset = 0;
			x_inc_en = 0;
			y_inc_en = 0;
		end
		BYTE1: begin
			pixel_data_RGB332 = 8'b0;
			signal_part_1 = GPIO_1_D[27:20];
			signal_part_1_en = 1'b1; 
			W_EN = 0;
			//x and y
			x_reset = 0;
			y_reset = 0;
			x_inc_en = 0;
			y_inc_en = 0;
		end
		BYTE2: begin
			//RBG Configuration
			pixel_data_RGB332 = {signal_part_1_out[7:5], GPIO_1_D[24:22], signal_part_1_out[2:1]};
			//pixel_data_RGB332 = 8'b11100000;
			//RGB configuration
			//pixel_data_RGB332 = {signal_part_1_out[7:5], signal_part_1_out[3:1], GPIO_1_D[24:21]};
			//just RB
			//pixel_data_RGB332 = {signal_part_1_out[7:4], GPIO_1_D[24:21]};
			//pixel_data_RGB332 = 8'b10010010;
			//reversed
			//pixel_data_RGB332 = {GPIO_1_D[27:25], signal_part_1[4:2], GPIO_1_D[22:21]};
			signal_part_1 = 8'd0;
			signal_part_1_en = 1'b0;
			W_EN = 1;
			//x and y
			x_reset = 0;
			y_reset = 0;
			x_inc_en = 1;
			y_inc_en = 0;
		end
		RESET: begin
			pixel_data_RGB332 = 8'b0;
			signal_part_1 = 8'b0;
			signal_part_1_en = 1'b1; 
			W_EN = 0;
			//x and y
			x_reset = 1;
			y_reset = 1;
			x_inc_en = 0;
			y_inc_en = 0;
		end
		LINE_OVER: begin
			pixel_data_RGB332 = 8'b0;
			signal_part_1 = 8'b0;
			signal_part_1_en = 1'b1; 
			W_EN = 0;
			//x and y
			x_reset = 1;
			y_reset = 0;
			x_inc_en = 0;
			y_inc_en = 1;
		end
		default: begin
			pixel_data_RGB332 = 8'b11111111;
			signal_part_1 = 8'd0;
			signal_part_1_en = 1'b0; 
			W_EN = 0;
			//x and y
			x_reset = 0;
			y_reset = 0;
			x_inc_en = 0;
			y_inc_en = 0;
		end
	endcase
end
//state update
always@ (posedge PCLK) begin
	state <= next_state;
	if (signal_part_1_en) signal_part_1_out <= signal_part_1;
	else signal_part_1_out <= 8'b0;
	if (x_reset) X_ADDR <= 15'b0;
	else if (x_inc_en) X_ADDR <= X_ADDR + 1;
	else X_ADDR <= X_ADDR;
	if (y_reset) Y_ADDR <= 15'b0;
	else if (y_inc_en) Y_ADDR <= Y_ADDR + 1;
	else Y_ADDR <= Y_ADDR;
end


/*

always @ (posedge c0_sig) begin
  if (HREF) begin
		//one byte comes in at each negedge
		//signal_part_1 stores the first 8 bytes
		if(count) begin
		  signal_part_1 <= GPIO_1_D[27:20];
		  //pixel_data_RGB332 <= 8'd0;
		  pixel_data_RGB332 <= count;
		  count <= 1'b0;
		  if (X_ADDR == 175) X_ADDR <= 15'b0;
		  W_EN <= 1'b0;
		end
		//signal_part_2 stores the second 8 bytes
		else begin
			signal_part_2 <= GPIO_1_D[27:20];
			//3 MSB of red
		   //pixel_data_RGB332 <= pixel_data_RGB332 | (signal_part_1[7:5] << 5);
			//3 MSB of blue
			//pixel_data_RGB332 <= pixel_data_RGB332 | (signal_part_2[4:2] << 2);
		   //2 MSB of green
		   //pixel_data_RGB332 <= pixel_data_RGB332 | signal_part_1[2:1];
			pixel_data_RGB332 <= 8'b11100000;
			count <= 1'b1;
			X_ADDR <= X_ADDR + 1;
			W_EN <= 1'b1;
		end
  end
end


always @ (negedge HREF) begin
//reset Y
	if (Y_ADDR == 143) Y_ADDR <= 0; 
	else begin
//increment Y
		Y_ADDR <= Y_ADDR + 1;
	end 
end
*/	
endmodule 