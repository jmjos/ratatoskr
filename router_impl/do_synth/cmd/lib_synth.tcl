# the local library for synthesis 

proc do_synth { UNIT_NAME  { T }  { TECH tcbn65lptc } {clk clk}  {rst rst}  }  {

    # Set-up the environment and the technology
    flow_setup_def
    flow_set_tech $TECH

    # Analyze and elaborate automatically. Params can be added using -param width=>32,ports=>8. Also possible to use a file
    analyze    -library work -autoread -recursive ../rtl -top $UNIT_NAME
    elaborate  -library work                                  $UNIT_NAME
    link
    check_design

    # Set constraints
    flow_def_rst     $rst
    flow_def_clock   $T  $clk 
    flow_def_timing  [expr $T/8]   [expr $T/8]
    check_timing

    # Synthesize
    compile                       ;# Run the synthesize
    #change_names -rules vhdl -hier -verbose -log_changes ./log/change_names.log

    # Write reports
    #set prefix ${UNIT_NAME}
    set prefix ${UNIT_NAME}_T=${T}_TECH=${TECH}   ;# Define prefix to identify reports. 
    flow_report_all     $prefix   ;# Write reports
    flow_write_netlist  $prefix   ;# Write results
}

