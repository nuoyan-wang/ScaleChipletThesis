set script_dir [file dirname [file normalize [info script]]]
set part "xcu55c-fsvh2892-2L-e"
set clk "3.333"

cd $script_dir
open_project -reset csynth_prj
set_top c0_router
add_files kernel.cpp
open_solution -reset solution1 -flow_target vivado
set_part $part
create_clock -period $clk -name default
set_clock_uncertainty 0.2 default
csynth_design
exit
