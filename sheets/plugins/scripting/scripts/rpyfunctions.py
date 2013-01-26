#!/usr/bin/env kross

"""
Sheets python script that provides access to the R programming environment for
data analysis and graphics ( http://www.r-project.org ) using the RPy python
module ( http://rpy.sourceforge.net ).

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/sheets
This script is licensed under the BSD license.
"""

import types, traceback
import Kross, KSpread

T = Kross.module("kdetranslation")

try:
    import rpy
except ImportError:
    raise "Failed to import the RPy python module. See http://rpy.sourceforge.net"

class Rfunctions:

    class Function:
        def __init__(self, functions, name, comment, format, params):
            self.__name__ = "R_%s" % name.upper()
            #self.__name__ = "R.%s" % name.upper()

            try:
                self.robj = getattr(rpy.r, name)
                self.isNewFunc = not KSpread.hasFunction(self.__name__)
                if self.isNewFunc:

                    def format2name(fchar):
                        if fchar == 'f': return 'Float'
                        if fchar == 'i': return 'Int'
                        if fchar == 's': return 'String'
                        if fchar == 'b': return 'Boolean'
                        raise "Unknown format char '%s'" % fchar

                    def format2value(fchar, value):
                        if fchar == 'f': return float(value or 0.0)
                        if fchar == 'i': return int(value or 0)
                        if fchar == 's': return "%s" % (value or '')
                        if fchar == 'b':
                            if type(value) == types.StringType:
                                v = value.strip().lower()
                                return bool( len(v)>0 and v!='0' and v!='no' and v!='false' )
                            else:
                                return bool(value)
                        raise "Unknown format char '%s' with value '%s'" % (fchar,value)

                    try:
                        req,opt = format.split('|')
                    except ValueError:
                        req,opt = [ format, [] ]

                    func = KSpread.function(self.__name__)

                    func.typeName = 'Float'
                    func.comment = comment

                    func.syntax = "%s(%s)" % (self.__name__,', '.join([ p.split(':')[0] for p in params ]))
                    #func.syntax = "%s(%s)" % (self.__name__,', '.join([ format2name(r) for r in req ]))

                    func.minparam = len(req)
                    func.maxparam = func.minparam + len(opt)
                    for i in range(0,len(req)):
                        func.addParameter(format2name(req[i]), params[i].split(':')[1])
                    for i in range(0,len(opt)):
                        func.addParameter(format2name(opt[i]), params[len(req)+i].split(':')[1])

                    def callback(arguments):
                        args = []
                        for i in range(0,len(req)):
                            args.append( format2value(req[i], arguments[i]) )
                        for i in range(0,len(opt)):
                            idx = len(req) + i + 1
                            if idx >= len(arguments):
                                break
                            args.append( format2value(opt[i], arguments[idx]) )

                        print "callback name=%s args=%s" % (self.__name__,args)

                        try:
                            func.result = self.robj(*args)
                            #func.result = getattr(self, name)(arguments)
                        except Exception, e:
                            func.error = "%s" % e
                    func.connect("called(QVariantList)", callback)
                    func.registerFunction()

                functions.append( self )
            except Exception, e:
                traceback.print_exc(file=sys.stderr)

        #def dnorm(self, args):
            #args = [1.2,2.3,3.4]
            #print "................................. %s" % args
            #x = float(args[0] or 0.0)
            #mu = float(args[1] or 0.0)
            #sigma = float(args[2] or 0.0)
            #give_log = (len(args)>=4 and args[3]) or False
            #return self.robj(x, mu, sigma, give_log)

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction

        self.functions = []
        F = Rfunctions.Function

        # the 45 functions the gnumeric plugin fn-r/functions.c provides.

        F(self.functions, 'dnorm', T.i18n('This function returns the probability density function of the normal distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"mu:" + T.i18n('mean of the distribution'),"sigma:" + T.i18n('standard deviation of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pnorm', T.i18n('This function returns the cumulative distribution function of the normal distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"mu:" + T.i18n('mean of the distribution'),"sigma:" + T.i18n('standard deviation of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qnorm', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the normal distribution.'),
            'fff|bb', ["p:" + T.i18n('probability'),"mu:" + T.i18n('mean of the distribution'),"sigma:" + T.i18n('standard deviation of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dlnorm', T.i18n('This function returns the probability density function of the log-normal distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"logmean:" + T.i18n('mean of the underlying normal distribution'),"logsd:" + T.i18n('standard deviation of the underlying normal distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'plnorm', T.i18n('This function returns the cumulative distribution function of the log-normal distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"logmean:" + T.i18n('mean of the underlying normal distribution'),"logsd:" + T.i18n('standard deviation of the underlying normal distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qlnorm', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the log-normal distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"logmean:" + T.i18n('mean of the underlying normal distribution'),"logsd:" + T.i18n('standard deviation of the underlying normal distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dgamma', T.i18n('This function returns the probability density function of the gamma distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"shape:" + T.i18n('the shape parameter of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pgamma', T.i18n('This function returns the cumulative distribution function of the gamma distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"shape:" + T.i18n('the shape parameter of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qgamma', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the gamma distribution.'),
            'fff|bb', ["p:" + T.i18n('probability'),"shape:" + T.i18n('the shape parameter of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dbeta', T.i18n('This function returns the probability density function of the beta distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"a:" + T.i18n('the first shape parameter of the distribution'),"b:" + T.i18n('the second scale parameter of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pbeta', T.i18n('This function returns the cumulative distribution function of the beta distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"a:" + T.i18n('the first shape parameter of the distribution'),"b:" + T.i18n('the second scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qbeta', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the beta distribution.'),
            'fff|bb', ["p:" + T.i18n('probability'),"a:" + T.i18n('the first shape parameter of the distribution'),"b:" + T.i18n('the second scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dt', T.i18n('This function returns the probability density function of the Student t distribution.'),
            'ff|b', ["x:" + T.i18n('observation'),"n:" + T.i18n('the number of degrees of freedom of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pt', T.i18n('This function returns the cumulative distribution function of the Student t distribution.'),
            'ff|bb', ["x:" + T.i18n('observation'),"n:" + T.i18n('the number of degrees of freedom of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qt', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Student t distribution.'),
            'ff|bb', ["p:" + T.i18n('probability'),"n:" + T.i18n('the number of degrees of freedom of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'df', T.i18n('This function returns the probability density function of the F distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"n1:" + T.i18n('the first number of degrees of freedom of the distribution'),"n2:" + T.i18n('the second number of degrees of freedom of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pf', T.i18n('This function returns the cumulative distribution function of the F distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"n1:" + T.i18n('the first number of degrees of freedom of the distribution'),"n2:" + T.i18n('the second number of degrees of freedom of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qf', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the F distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"n1:" + T.i18n('the first number of degrees of freedom of the distribution'),"n2:" + T.i18n('the second number of degrees of freedom of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dchisq', T.i18n('This function returns the probability density function of the chi-square distribution.'),
            'ff|b', ["x:" + T.i18n('observation'),"df:" + T.i18n('the number of degrees of freedom of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pchisq', T.i18n('This function returns the cumulative distribution function of the chi-square distribution.'),
            'ff|bb', ["x:" + T.i18n('observation'),"df:" + T.i18n('the number of degrees of freedom of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qchisq', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the chi-square distribution.'),
            'ff|bb', ["p:" + T.i18n('probability'),"df:" + T.i18n('the number of degrees of freedom of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dweibull', T.i18n('This function returns the probability density function of the Weibull distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"shape:" + T.i18n('the shape parameter of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pweibull', T.i18n('This function returns the cumulative distribution function of the Weibull distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"shape:" + T.i18n('the shape parameter of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qweibull', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Weibull distribution.'),
            'fff|bb', ["p:" + T.i18n('probability'),"shape:" + T.i18n('the shape parameter of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dpois', T.i18n('This function returns the probability density function of the Poisson distribution.'),
            'ff|b', ["x:" + T.i18n('observation'),"lambda:" + T.i18n('the mean of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'ppois', T.i18n('This function returns the cumulative distribution function of the Poisson distribution.'),
            'ff|bb', ["x:" + T.i18n('observation'),"lambda:" + T.i18n('the mean of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qpois', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Poisson distribution.'),
            'ff|bb', ["p:" + T.i18n('probability'),"lambda:" + T.i18n('the mean of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dexp', T.i18n('This function returns the probability density function of the exponential distribution.'),
            'ff|b', ["x:" + T.i18n('observation'),"scale:" + T.i18n('the scale parameter of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pexp', T.i18n('This function returns the cumulative distribution function of the exponential distribution.'),
            'ff|bb', ["x:" + T.i18n('observation'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qexp', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the exponential distribution.'),
            'ff|bb', ["p:" + T.i18n('probability'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dbinom', T.i18n('This function returns the probability density function of the binomial distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"n:" + T.i18n('the number of trials'),"psuc:" + T.i18n('the probability of success in each trial'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pbinom', T.i18n('This function returns the cumulative distribution function of the binomial distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"n:" + T.i18n('the number of trials'),"psuc:" + T.i18n('the probability of success in each trial'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qbinom', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the binomial distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"n:" + T.i18n('the number of trials'),"psuc:" + T.i18n('the probability of success in each trial'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dnbinom', T.i18n('This function returns the probability density function of the negative binomial distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"n:" + T.i18n('the number of trials'),"psuc:" + T.i18n('the probability of success in each trial'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pnbinom', T.i18n('This function returns the cumulative distribution function of the negative binomial distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"n:" + T.i18n('the number of trials'),"psuc:" + T.i18n('the probability of success in each trial'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qnbinom', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the negative binomial distribution.'),
            'fff|bb', ["p:" + T.i18n('probability'),"n:" + T.i18n('the number of trials'),"psuc:" + T.i18n('the probability of success in each trial'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dhyper', T.i18n('This function returns the probability density function of the hypergeometric distribution.'),
            'ffff|b', ["x:" + T.i18n('observation'),"r:" + T.i18n('the number of red balls'),"b:" + T.i18n('the number of black balls'),"n:" + T.i18n('the number of balls drawn'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'phyper', T.i18n('This function returns the cumulative distribution function of the hypergeometric distribution.'),
            'ffff|bb', ["x:" + T.i18n('observation'),"r:" + T.i18n('the number of red balls'),"b:" + T.i18n('the number of black balls'),"n:" + T.i18n('the number of balls drawn'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qhyper', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the hypergeometric distribution.'),
            'ffff|bb', ["p:" + T.i18n('probability'),"r:" + T.i18n('the number of red balls'),"b:" + T.i18n('the number of black balls'),"n:" + T.i18n('the number of balls drawn'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dgeom', T.i18n('This function returns the probability density function of the geometric distribution.'),
            'ff|b', ["x:" + T.i18n('observation'),"psuc:" + T.i18n('the probability of success in each trial'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pgeom', T.i18n('This function returns the cumulative distribution function of the geometric distribution.'),
            'ff|bb', ["x:" + T.i18n('observation'),"psuc:" + T.i18n('the probability of success in each trial'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qgeom', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the geometric distribution.'),
            'ff|bb', ["p:" + T.i18n('probability'),"psuc:" + T.i18n('the probability of success in each trial'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        F(self.functions, 'dcauchy', T.i18n('This function returns the probability density function of the Cauchy distribution.'),
            'fff|b', ["x:" + T.i18n('observation'),"location:" + T.i18n('the center of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"give_log:" + T.i18n('If true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'pcauchy', T.i18n('This function returns the cumulative distribution function of the Cauchy distribution.'),
            'fff|bb', ["x:" + T.i18n('observation'),"location:" + T.i18n('the center of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )
        F(self.functions, 'qcauchy', T.i18n('This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Cauchy distribution.'),
            'fff|bb', ["p:" + T.i18n('probability'),"location:" + T.i18n('the center of the distribution'),"scale:" + T.i18n('the scale parameter of the distribution'),"lower_tail:" + T.i18n('If true (the default), the lower tail of the distribution is considered.'),"log_p:" + T.i18n('If true, log of the probability is used. This is useful if the probability would otherwise underflow to 0. Defaults to false.')] )

        print "########################################################################################"

#print "Started Y! Finance"
Rfunctions( self )
#print "Finished Y! Finance"
