import QuantLib as ql

# QuoteHandle 
def QuoteHandle(quote):
	quote_handle = ql.QuoteHandle(ql.SimpleQuote(quote))
	return quote_handle
