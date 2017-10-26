# Floorplan 2-Kernel design for lower and upper SLR
current_instance -quiet
add_cells_to_pblock pblock_upper [get_cells xcl_design_i/expanded_region/u_ocl_region/dr_i/sqlq6tpch1/inst] 
add_cells_to_pblock pblock_lower [get_cells xcl_design_i/expanded_region/u_ocl_region/dr_i/sqlq6tpch0/inst]
