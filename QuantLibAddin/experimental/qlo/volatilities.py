import QuantLib as ql

# BlackVolTermStructureHandle 
def BlackVolTermStructureHandle(valuation_date, volatility):
	vol_ts = ql.BlackVolTermStructureHandle(
        	ql.BlackConstantVol(
            	valuation_date,
            	ql.NullCalendar(),
            	volatility,
            	ql.Actual365Fixed()
        	)
    	)
	return vol_ts
