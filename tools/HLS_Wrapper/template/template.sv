`timescale 1 ns / 1 ps

module wrapper_tf(
//<<module_io>>
);

//<<reg_wire_dec>>

    coord_delay_tl UUT (
//<<tl_conn>>
    );

    integer i;
    integer j;
    integer k;
    integer l;

    always @(posedge clock) begin
        if (reset == 1'h1) begin
//<<rst_logic>>
        end else begin
//<<shift_logic>>
        end
    end

endmodule