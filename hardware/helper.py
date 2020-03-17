#       Return port number
def ret_port_num(noc_x,noc_y, noc_z, x, y, z):
  port_num=7
  if (y == noc_y-1):
    port_num-=1
  if (x == noc_x-1):
    port_num-=1
  if (y == 0):
    port_num-=1
  if (x == 0):
    port_num-=1
  if (z == noc_z-1):
    port_num-=1
  if (z == 0):
    port_num-=1
  return port_num
#########################################################################################
#	Number of Virtual channels for Each Port of Router
def ret_int_array(num, val):
  int_array = "("
  if num is 1:
    int_array += "0 => "
  for i in range(num):
    int_array+=str(val)
    int_array+=" ,"
  int_array=int_array[:-1]
  int_array+=")"
  return int_array
#########################################################################################
#	Buffer Depth of Each Virtual Channel of Each Port
def ret_2D_int_array(num1, num2, val):
  int_array_2D = "("
  for i in range(num1):
    int_array_2D+=ret_int_array(num2, val)
    int_array_2D+=" ,"
  int_array_2D=int_array_2D[:-1]
  int_array_2D+=")"
  return int_array_2D
#########################################################################################
def ret_io_sig_lb(x,y,z,noc_x,noc_y,noc_z,vc_xy):
  io_sig_var=0
  if z==0:
    io_sig_var= (x+(y*noc_x))*vc_xy[z]
  else:
    for i in range(z):
      io_sig_var+=ret_layer_io_boundry(x=noc_x,y=noc_y,vc_num=vc_xy[i])
    io_sig_var+= (x+(y*noc_x))*vc_xy[z]
  return io_sig_var
#########################################################################################
def ret_layer_io_boundry(x,y,vc_num):
  return x*y*vc_num
#########################################################################################
#	Return Sum of VCs of a router
def ret_sum_vc(z,vc_xy,vc_z,noc_z,port_num):
  sum_vc_var=0
  if z == 0 :
    sum_vc_var=vc_z[z]+(port_num-1)*vc_xy[z]
  else:
    if z == noc_z-1 :
      sum_vc_var = vc_z[z-1]+(port_num-1)*vc_xy[z]
    else:
      sum_vc_var = vc_z[z-1]+vc_z[z]+(port_num-2)*vc_xy[z]
  return sum_vc_var
#########################################################################################
def ret_port_len(cf_vec, noc_z, z, port_num):
  if cf_vec[z]== 1:
    port_len=port_num
  elif z == noc_z-1:
    port_len=cf_vec[z]+cf_vec[z-1]+port_num-2
  else:
    port_len=2*cf_vec[z]+cf_vec[z-1]+port_num-3
  return port_len
#########################################################################################
def ret_local_lb(x,y,z, noc_x, noc_y, noc_z, cf_vec, flit_size):
  local_lb=0
  if z==0:
    local_lb= (x+(y*noc_x))*cf_vec[z]*flit_size
  else:
    for i in range(z):
      local_lb+=noc_x*noc_y*cf_vec[i]*flit_size
    local_lb+= (x+(y*noc_x))*cf_vec[z]*flit_size
  return local_lb
