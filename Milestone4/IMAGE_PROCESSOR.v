`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144
`define NUM_BARS 3
`define BAR_HEIGHT 48


//------------------------------------------------------------------------
// 1r1w register file with reset
//------------------------------------------------------------------------

module vc_ResetRegfile_1r1w
#(
  parameter p_data_nbits  = 1,
  parameter p_num_entries = 2,
  parameter p_reset_value = 0,

  // Local constants not meant to be set from outside the module
  parameter c_addr_nbits  = $clog2(p_num_entries)
)(
  input  wire                    clk,
  input  wire                   reset,

  // Read port (combinational read)

  input  wire [c_addr_nbits-1:0] read_addr,
  output wire [p_data_nbits-1:0] read_data,

  // Write port (sampled on the rising clock edge)

  input  wire                    write_en,
  input  wire [c_addr_nbits-1:0] write_addr,
  input  wire [p_data_nbits-1:0] write_data
);

  wire [p_data_nbits-1:0] rfile[p_num_entries-1:0];

  // Combinational read

  assign read_data = rfile[read_addr];

  // Write on positive clock edge. We have to use a generate statement to
  // allow us to include the reset logic for each individual register.

  genvar i;
  generate
    for ( i = 0; i < p_num_entries; i = i+1 )
    begin : wport
      always_ff @( posedge clk )
        if ( reset )
          rfile[i] <= p_reset_value;
        else if ( write_en && (i[c_addr_nbits-1:0] == write_addr) )
          rfile[i] <= write_data;
    end
  endgenerate

endmodule

	//------------------------------------------------------------------------
	// Postive-edge triggered flip-flop with enable and reset
	//------------------------------------------------------------------------

	module vc_EnResetReg
	#(
	  parameter p_nbits       = 1,
	  parameter p_reset_value = 0
	)(
	  input  wire               clk,   // Clock input
	  input  wire               reset, // Sync reset input
	  output wire [p_nbits-1:0] q,     // Data output
	  input  wire [p_nbits-1:0] d,     // Data input
	  input  wire               en     // Enable input
	);

	  always_ff @( posedge clk )
	    if ( reset || en )
	      q <= reset ? p_reset_value : d;

endmodule


module IMAGE_PROCESSOR (
	PIXEL_IN,
	CLK,
	VGA_PIXEL_X,
	VGA_PIXEL_Y,
	VGA_VSYNC_NEG,
	RESULT,
);
//--------------------------------------------------------
//Wires
//--------------------------------------------------------
wire[4:0] state;
wire[4:0] next_state;
wire reset;

wire is_Red;
wire is_Blue;

wire[9:0] row_count_in;
wire[9:0] row_count_out;
wire 			row_count_en;
wire[9:0] start_x_in;
wire[9:0] start_x_out;
wire 			start_x_en;
wire[9:0] end_x_in;
wire[9:0] end_x_out;
wire 			end_x_en;
wire[9:0] start_y_in;
wire[9:0] start_y_out;
wire 			start_y_en;
wire[9:0] end_y_in;
wire[9:0] end_y_out;
wire 			end_y_en;
wire[9:0] final_count_in;
wire[9:0] final_count_out;
wire 			final_count_en;
wire[9:0] prev_width_in;
wire[9:0] prev_width_out;
wire 			prev_width_en;
wire[2:0] output_buffer_in;
wire[2:0] output_buffer_out;
wire 			output_buffer_en;

wire[7:0] width_read_addr;
wire[9:0] width_out;
wire 			width_wen;
wire[9:0] width_in;
//--------------------------------------------------------
//Regs and Regfile Declarations
//--------------------------------------------------------
vc_EnResetReg #(10, 0) row_count
(
	.clk(CLK),
	.reset(reset),
	.q(row_count_in),
	.d(row_count_out),
	.en(row_count_en)
);
vc_EnResetReg #(10, 0) start_x
(
	.clk(CLK),
	.reset(reset),
	.q(start_x_in),
	.d(start_x_out),
	.en(start_x_en)
);
vc_EnResetReg #(10, 0) end_x
(
	.clk(CLK),
	.reset(reset),
	.q(end_x_in),
	.d(end_x_out),
	.en(end_x_en)
);
vc_EnResetReg #(10, 0) start_y
(
	.clk(CLK),
	.reset(reset),
	.q(start_y_in),
	.d(start_y_out),
	.en(start_y_en)
);
vc_EnResetReg #(10, 0) end_y
(
	.clk(CLK),
	.reset(reset),
	.q(end_y_in),
	.d(end_y_out),
	.en(end_y_en)
);
vc_EnResetReg #(10, 0) final_count
(
	.clk(CLK),
	.reset(reset),
	.q(final_count_in),
	.d(final_count_out),
	.en(final_count_en)
);
vc_EnResetReg #(10, 0) prev_width
(
	.clk(CLK),
	.reset(reset),
	.q(prev_width_in),
	.d(prev_width_out),
	.en(prev_width_en)
);
vc_EnResetReg #(10, 0) output_buffer
(
	.clk(CLK),
	.reset(reset),
	.q(output_buffer_in),
	.d(output_buffer_out),
	.en(output_buffer_en)
);

vc_ResetRegfile_1r1w (10, SCREEN_HEIGHT, 0) Width_Array
(
	.clk(CLK),
	.reset(reset),

	.read_addr(width_read_addr),
	.read_data(width_out),
	.write_en(width_wen),
	.write_addr([7:0]VGA_PIXEL_Y),
	.write_data(width_in)
);
//=======================================================
//  PORT declarations
//=======================================================
input	[7:0]	PIXEL_IN;
input 		CLK;

input [9:0] VGA_PIXEL_X;
input [9:0] VGA_PIXEL_Y;
input			VGA_VSYNC_NEG;

output [2:0] RESULT;

//---------------------------------------------------------
//State Definitions
//---------------------------------------------------------
localparam IDLE = 						5'b00000;
localparam Start_Red_Shape = 	5'b00001;
localparam First_Red_Found = 	5'b00010;
localparam Red_Found = 				5'b00011;
localparam Idle_On_Red_line =	5'b00100;
localparam Line_Over_Red = 		5'b00101;
localparam Next_Line_Red = 		5'b00110;

localparam Final_Calc_1 = 		5'b00111;
localparam Final_Calc_2 = 		5'b01000;
localparam Final_Calc_Tri = 	5'b01001;
localparam Final_Calc_Dia = 	5'b01010;
localparam Final_Calc_Squ = 	5'b01011;
localparam Output_Tri = 			5'b01101;
localparam Output_Dia = 			5'b01110;
localparam Output_Squ = 			5'b01111;

localparam Start_Blu_Shape = 	5'b10001;
localparam First_Blu_Found = 	5'b10010;
localparam Blu_Found = 				5'b10011;
localparam Idle_On_Blu_Line =	5'b10100;
localparam Line_Over_Blu = 		5'b10101;
localparam Next_Line_Blu = 		5'b10110;

localparam RESET = 						5'b11111;

//---------------------------------------------------------
//Red/Blue Assigns
//---------------------------------------------------------
//has a lot of red, not a lot of blue or green
assign isRed = (input[7:5] > 101 &  input[4:2] < 100 & input[1:0] < 10)
//has a lot of blue, not a lot of red or green
assign isRed = (input[4:2] > 101 &  input[7:5] < 100 & input[1:0] < 10)

//---------------------------------------------------------
//State Transitions
//---------------------------------------------------------
always @ * begin
	case (state)
		IDLE: begin
			if (is_Blue) next_state = Start_Blu_Shape;
			else if (is_Red) next_state = Start_Red_Shape;
			else next_state = IDLE;
		end
		Start_Red_Shape: begin
			if (is_Red) next_state = Red_Found;
			else next_state = Idle_On_Red_line;
		end
		First_Red_Found: begin
			if (is_Red) next_state = Red_Found;
			else next_state = Idle_On_Red_line;
		end
		Red_Found: begin
			if (VGA_PIXEL_X == SCREEN_WIDTH-1) next_state = Line_Over_Red;
			else if (is_Red) next_state = Red_Found;
			else next_state = Idle_On_Red_line;
		end
		Idle_On_Red_line: begin
			if (VGA_PIXEL_X == SCREEN_WIDTH-1) next_state = Line_Over_Red;
			else if (is_Red) next_state = Red_Found;
			else next_state = Line_Over_Red;
		end
		Line_Over_Red: begin
			//last line
			if (VGA_PIXEL_Y == 10'b0) next_state = Final_Calc_1;
			else next_state = Next_Line_Red;
		end
		Next_Line_Red: begin
			if (is_Red) next_state = First_Red_Found;
			//line passes with no red
			else if (end_y_out+1 < (VGA_PIXEL_Y)) next_state = RESET;
			else next_state = Next_Line_Red;
		end
		Final_Calc_1: next_state = Final_Calc_2;
		Final_Calc_2: begin
			//increasing
			if if (width_out == prev_width_out) next_state = Final_Calc_Squ;
			//about the sample
			else (width_out > prev_width_out) next_state = Final_Calc_Tri;
			else next_state = RESET;
		end
		Final_Calc_Tri: begin
			//increasing
			if (width_out >= prev_width_out) next_state = Final_Calc_Tri;
			//decreasing
			else if (width_out < prev_width_out) next_state = Final_Calc_Dia;
			//end of array
			else if (final_count_out == end_y_out) next_state = Output_Tri;
			else next_state = RESET;
		end
		Final_Calc_Squ: begin
			//~ same
			if (width_out == prev_width_out) next_state = Final_Calc_Squ;
			//end of array
			else if (final_count_out == end_y_out) next_state = Output_Squ;
			else next_state = RESET;
		end
		Final_Calc_Dia: begin
			//decreasing
			if (width_out < prev_width_out) next_state = Final_Calc_Dia;
			//end of array
			else if (final_count_out == end_y_out) next_state = Output_Dia;
			else next_state = RESET;
		end
		Output_Dia: next_state = RESET;
		Output_Squ: next_state = RESET;
		Output_Tri: next_state = RESET;
		RESET: next_state = IDLE;
		default: begin
			next_state = IDLE;
		end
	endcase
end

//---------------------------------------------------------
//State Outputs
//---------------------------------------------------------
always @ (*) begin
	case(state) begin
		IDLE: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Start_Red_Shape: begin
			//enables
			row_count_en =   	 1'b1;
			start_x_en = 			 1'b1;
			end_x_en; =   		 1'b1;
			start_y_en = 			 1'b1;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b1;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b1;
			start_x_in = 			 VGA_PIXEL_X;
			end_x_in; = 			 VGA_PIXEL_X;
			start_y_in = 			 VGA_PIXEL_Y;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		First_Red_Found: begin
			//enables
			row_count_en =   	 1'b1;
			start_x_en = 			 1'b1;
			end_x_en; =   		 1'b1;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b1;
			start_x_in = 			 VGA_PIXEL_X;
			end_x_in; = 			 VGA_PIXEL_X;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Red_Found: begin
			//enables
			row_count_en =   	 1'b1;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b1;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 row_count_out + 1;
			start_x_in = 			 10'b0;
			end_x_in; = 			 VGA_PIXEL_X;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Idle_On_Red_line: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Line_Over_Red: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b1;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b1;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 VGA_PIXEL_Y;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = end_x_out - start_x_out;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Next_Line_Red: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b1;
			end_x_en; =   		 1'b1;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Final_Calc_1: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b1;
			prev_width_en =    1'b1;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 start_y_out + 1;
			prev_width_in =  	 width_out;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  start_y_out[7:0];
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Final_Calc_2: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b1;
			prev_width_en =    1'b1;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 final_count_out + 1;
			prev_width_in =  	 width_out;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  final_count_out[7:0];
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Final_Calc_Tri: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b1;
			prev_width_en =    1'b1;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 final_count_out + 1;
			prev_width_in =  	 width_out;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  final_count_out[7:0];
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Final_Calc_Squ: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b1;
			prev_width_en =    1'b1;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 final_count_out + 1;
			prev_width_in =  	 width_out;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  final_count_out[7:0];
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Final_Calc_Dia: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b1;
			prev_width_en =    1'b1;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 final_count_out + 1;
			prev_width_in =  	 width_out;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  final_count_out[7:0];
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
		Output_Tri: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  8'b0;
			//misc
			reset = 1'b1;
			if (end_y_out - start_y_out > 7) RESULT = output_buffer_out && //Triangle Code//;
			else RESULT = 3'b0;
		end
		Output_Squ: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  8'b0;
			//misc
			reset = 1'b1;
			if (end_y_out - start_y_out > 7) RESULT = output_buffer_out && //Square Code//;
			else RESULT = 3'b0;
		end
		Output_Dia: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =  8'b0;
			//misc
			reset = 1'b1;
			if (end_y_out - start_y_out > 7) RESULT = output_buffer_out && //Diamond Code//;
			else RESULT = 3'b0;
		end
		RESET: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b0;
			RESULT = 3'b0;
		end
		default: begin
			//enables
			row_count_en =   	 1'b0;
			start_x_en = 			 1'b0;
			end_x_en; =   		 1'b0;
			start_y_en = 			 1'b0;
			end_y_en = 				 1'b0;
			final_count_en = 	 1'b0;
			prev_width_en =    1'b0;
			output_buffer_en = 1'b0;
			width_wen = 			 1'b0;
			//inputs
			row_count_in = 		 10'b0;
			start_x_in = 			 10'b0;
			end_x_in; = 			 10'b0;
			start_y_in = 			 10'b0;
			end_y_in = 				 10'b0;
			final_count_in = 	 10'b0;
			prev_width_in =  	 10'b0;
			output_buffer_in = 3'b0;
			width_in 				 = 10'b0;
			width_read_addr =   8'b0;
			//misc
			reset = 1'b1;
			RESULT = 3'b0;
		end
	endcase
end
//---------------------------------------------------------
//Clocking
//---------------------------------------------------------
always @ posedge(CLK) begin
	state <= next_state;
end

endmodule
