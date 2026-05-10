import QuantLib as ql

# FlatForward
def YieldTermStructureHandle(valuation_date, quote, day_counter):
	ts_handle = ql.YieldTermStructureHandle( ql.FlatForward(
		valuation_date,
		quote,
		day_counter
        	))
	return ts_handle 
