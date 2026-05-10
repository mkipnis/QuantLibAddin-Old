import QuantLib as ql

# BlackScholesMertonProcess 
def BlackScholesMertonProcess(spot_handle, div_yield_ts, rfr_ts, vol_ts):

	process = ql.BlackScholesMertonProcess(
            	spot_handle, div_yield_ts, rfr_ts, vol_ts
        	)

	return process 
