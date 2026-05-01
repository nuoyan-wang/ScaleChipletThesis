set script_dir [file dirname [file normalize [info script]]]
set part "xcu55c-fsvh2892-2L-e"
set clk "3.333"

cd $script_dir
open_project -reset p
set_top chunk2_stream_ip
add_files stream_ip.cpp
open_solution -reset s -flow_target vitis
set_part $part
create_clock -period $clk -name default
set_clock_uncertainty 0.2 default
csynth_design
config_export \
  -flow impl \
  -vivado_clock $clk \
  -vivado_impl_strategy Performance_ExplorePostRoutePhysOpt \
  -vivado_optimization_level 3 \
  -vivado_phys_opt all \
  -vivado_report_level 2 \
  -vivado_synth_strategy Flow_PerfOptimized_high \
  -vivado_synth_design_args {-directive sdx_optimization_effort_high}
export_design -format xo -output c2.xo
exit
