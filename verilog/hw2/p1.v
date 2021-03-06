/* HW2, P1 */

module Combo_str(output Y, input A, B, C, D);
	wire n, m, p, q;

	not	not1(p, D),
		not2(m,n);
	or	or1(n, A, D);
	and	and1(q, B, C, p),
		and2(Y, q, m);
endmodule

module t_Combo_str();
	/* Try every possible input an show all outputs */
	reg A,B,C,D;
	wire Y;
	integer inp;

	reg [3:0] r;
	Combo_str X(Y,A,B,C,D);

	initial begin
		{A,B,C,D} = 'b0000;
		$dumpfile("p1.vcd");
		$dumpvars(1, X);
		$monitor("A=%b B=%b, C=%b, D=%b, Y=%b",A,B,C,D,Y);
		for(inp = 0; inp <= 'b1111; inp = inp + 1) begin
			#10 {A,B,C,D} = inp[3:0];
		end
		$finish();
	end

endmodule
