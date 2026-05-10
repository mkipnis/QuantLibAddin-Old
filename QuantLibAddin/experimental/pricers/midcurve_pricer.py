import json
import QuantLib as ql

from concurrent.futures import ThreadPoolExecutor


def bootstrap(swap_index, curve_components):

    rate_helpers = []

    for curve_component in curve_components:

        if curve_component['Type'] == 'Swap':
            period = ql.Period(curve_component["Tenor"])
            quote = ql.SimpleQuote(curve_component["Quote"])

            rate_helpers.append(
                ql.OISRateHelper(
                    curve_component["SettlementDays"],
                    period,
                    ql.QuoteHandle(quote),
                    swap_index
                )
            )

    today = ql.Settings.instance().evaluationDate

    curve = ql.PiecewiseLogLinearDiscount(
        today,
        rate_helpers,
        ql.Actual360()
    )
    curve.enableExtrapolation()

    return ql.YieldTermStructureHandle(curve)


def price_mid_curves(index, forecast_curve, swap_tenors, forward_start_tenors):

    ois_midcurves_results = {}

    index_class = getattr(ql, index)
    index_obj = index_class(forecast_curve)

    for curve_tenor in swap_tenors:
        ois_midcurves = {}

        for forward_start in forward_start_tenors:
            ois = ql.MakeOIS(ql.Period(curve_tenor), index_obj, fixedRate=0.0001,
                             fwdStart=ql.Period(forward_start)
                             )
            ois.setPricingEngine(ql.DiscountingSwapEngine(forecast_curve))
            ois_midcurves[forward_start] = ois

        def calculate_fair_rate(item):
            forward_start, ois = item
            return forward_start, round(ois.fairRate(), 4)

        with ThreadPoolExecutor() as executor:
            midcurve_results = dict(
                executor.map(
                    calculate_fair_rate,
                    ois_midcurves.items()
                )
            )

        ois_midcurves_results[curve_tenor] = midcurve_results

    return ois_midcurves_results



def PriceMidCurves(price_request_str):

    price_request = json.loads(price_request_str)

    index_name = price_request["Index"]
    index_class = getattr(ql, index_name)
    swap_index = index_class()
    print(swap_index)

    curve_components = price_request['CurveComponents']
    curve_linear_discount = bootstrap(swap_index, curve_components)

    swap_tenor = price_request["swap_tenor"]
    forward_tenor = price_request["forward_tenor"]

    mid_curves = price_mid_curves(index_name, curve_linear_discount, swap_tenor, forward_tenor)

    return json.dumps(mid_curves)


# Example usage
if __name__ == "__main__":

    with open("mid_curve_price_request.json", "r", encoding="utf-8") as f:
        json_string = f.read()
        mid_curves = PriceMidCurves(json_string)
        print(mid_curves)
