#!/usr/bin/env kross

"""
KSpread python script that provides access to the R programming environment for
data analysis and graphics ( http://www.r-project.org ) using the RPy python
module ( http://rpy.sourceforge.net ).

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/kspread
This script is licensed under the BSD license.
"""

import types, traceback
import Kross, KSpread

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

        F(self.functions, 'dnorm', 'This function returns the probability density function of the normal distribution.',
            'fff|b', ["x:observation.","mu:mean of the distribution.","sigma:standard deviation of the distribution.","give_log:if true, log of the result will be returned instead. This is useful if the result would otherwise underflow to 0. Defaults to false."] )
        F(self.functions, 'pnorm', 'This function returns the cumulative distribution function of the normal distribution.',
            'fff|bb', ["x:observation.","mu:mean of the distribution.","sigma:standard deviation of the distribution.","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0. Defaults to false."] )
        F(self.functions, 'qnorm', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the normal distribution.',
            'fff|bb', ["p:probability.","mu:mean of the distribution.","sigma:standard deviation of the distribution.","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dlnorm', 'This function returns the probability density function of the log-normal distribution.',
            'fff|b', ["x:observation.","logmean:mean of the underlying normal distribution.","logsd:standard deviation of the underlying normal distribution.","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'plnorm', 'This function returns the cumulative distribution function of the log-normal distribution.',
            'fff|bb', ["x:observation.","logmean:mean of the underlying normal distribution.","logsd:standard deviation of the underlying normal distribution.","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qlnorm', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the log-normal distribution.',
            'fff|bb', ["x:observation.","logmean:mean of the underlying normal distribution.","logsd:standard deviation of the underlying normal distribution.","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dgamma', 'This function returns the probability density function of the gamma distribution.',
            'fff|b', ["x:observation.","shape:the shape parameter of the distribution","scale:the scale parameter of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pgamma', 'This function returns the cumulative distribution function of the gamma distribution.',
            'fff|bb', ["x:observation.","shape:the shape parameter of the distribution","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qgamma', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the gamma distribution.',
            'fff|bb', ["p:probability.","shape:the shape parameter of the distribution","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dbeta', 'This function returns the probability density function of the beta distribution.',
            'fff|b', ["x:observation.","a:the first shape parameter of the distribution","b:the second scale parameter of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pbeta', 'This function returns the cumulative distribution function of the beta distribution.',
            'fff|bb', ["x:observation.","a:the first shape parameter of the distribution","b:the second scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qbeta', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the beta distribution.',
            'fff|bb', ["p:probability.","a:the first shape parameter of the distribution","b:the second scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dt', 'This function returns the probability density function of the Student t distribution.',
            'ff|b', ["x:observation.","n:the number of degrees of freedom of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pt', 'This function returns the cumulative distribution function of the Student t distribution.',
            'ff|bb', ["x:observation.","n:the number of degrees of freedom of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qt', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Student t distribution.',
            'ff|bb', ["p:probability.","n:the number of degrees of freedom of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'df', 'This function returns the probability density function of the F distribution.',
            'fff|b', ["x:observation.","n1:the first number of degrees of freedom of the distribution","n2:the second number of degrees of freedom of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pf', 'This function returns the cumulative distribution function of the F distribution.',
            'fff|bb', ["x:observation.","n1:the first number of degrees of freedom of the distribution","n2:the second number of degrees of freedom of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qf', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the F distribution.',
            'fff|bb', ["x:observation.","n1:the first number of degrees of freedom of the distribution","n2:the second number of degrees of freedom of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dchisq', 'This function returns the probability density function of the chi-square distribution.',
            'ff|b', ["x:observation.","df:the number of degrees of freedom of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pchisq', 'This function returns the cumulative distribution function of the chi-square distribution.',
            'ff|bb', ["x:observation.","df:the number of degrees of freedom of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qchisq', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the chi-square distribution.',
            'ff|bb', ["p:probability.","df:the number of degrees of freedom of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dweibull', 'This function returns the probability density function of the Weibull distribution.',
            'fff|b', ["x:observation.","shape:the shape parameter of the distribution","scale:the scale parameter of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pweibull', 'This function returns the cumulative distribution function of the Weibull distribution.',
            'fff|bb', ["x:observation.","shape:the shape parameter of the distribution","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qweibull', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Weibull distribution.',
            'fff|bb', ["p:probability.","shape:the shape parameter of the distribution","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dpois', 'This function returns the probability density function of the Poisson distribution.',
            'ff|b', ["x:observation.","lambda:the mean of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'ppois', 'This function returns the cumulative distribution function of the Poisson distribution.',
            'ff|bb', ["x:observation.","lambda:the mean of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qpois', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Poisson distribution.',
            'ff|bb', ["p:probability.","lambda:the mean of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dexp', 'This function returns the probability density function of the exponential distribution.',
            'ff|b', ["x:observation.","scale:the scale parameter of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pexp', 'This function returns the cumulative distribution function of the exponential distribution.',
            'ff|bb', ["x:observation.","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qexp', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the exponential distribution.',
            'ff|bb', ["p:probability.","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dbinom', 'This function returns the probability density function of the binomial distribution.',
            'fff|b', ["x:observation.","n:the number of trials","psuc:the probability of success in each trial","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pbinom', 'This function returns the cumulative distribution function of the binomial distribution.',
            'fff|bb', ["x:observation.","n:the number of trials","psuc:the probability of success in each trial","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qbinom', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the binomial distribution.',
            'fff|bb', ["x:observation.","n:the number of trials","psuc:the probability of success in each trial","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dnbinom', 'This function returns the probability density function of the negative binomial distribution.',
            'fff|b', ["x:observation.","n:the number of trials","psuc:the probability of success in each trial","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pnbinom', 'This function returns the cumulative distribution function of the negative binomial distribution.',
            'fff|bb', ["x:observation.","n:the number of trials","psuc:the probability of success in each trial","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qnbinom', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the negative binomial distribution.',
            'fff|bb', ["p:probability.","n:the number of trials","psuc:the probability of success in each trial","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dhyper', 'This function returns the probability density function of the hypergeometric distribution.',
            'ffff|b', ["x:observation.","r:the number of red balls","b:the number of black balls","n:the number of balls drawn","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'phyper', 'This function returns the cumulative distribution function of the hypergeometric distribution.',
            'ffff|bb', ["x:observation.","r:the number of red balls","b:the number of black balls","n:the number of balls drawn","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qhyper', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the hypergeometric distribution.',
            'ffff|bb', ["p:probability.","r:the number of red balls","b:the number of black balls","n:the number of balls drawn","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dgeom', 'This function returns the probability density function of the geometric distribution.',
            'ff|b', ["x:observation.","psuc:the probability of success in each trial","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pgeom', 'This function returns the cumulative distribution function of the geometric distribution.',
            'ff|bb', ["x:observation.","psuc:the probability of success in each trial","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qgeom', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the geometric distribution.',
            'ff|bb', ["p:probability.","psuc:the probability of success in each trial","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        F(self.functions, 'dcauchy', 'This function returns the probability density function of the Cauchy distribution.',
            'fff|b', ["x:observation.","location:the center of the distribution","scale:the scale parameter of the distribution","give_log:if true, log of the result will be returned instead.  This is useful if the result would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'pcauchy', 'This function returns the cumulative distribution function of the Cauchy distribution.',
            'fff|bb', ["x:observation.","location:the center of the distribution","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )
        F(self.functions, 'qcauchy', 'This function returns the probability quantile function, i.e., the inverse of the cumulative distribution function, of the Cauchy distribution.',
            'fff|bb', ["p:probability.","location:the center of the distribution","scale:the scale parameter of the distribution","lower_tail:if true (the default), the lower tail of the distribution is considered.","log_p:if true, log of the probability is used.  This is useful if the probability would otherwise underflow to 0.  Defaults to false."] )

        print "########################################################################################"

#print "Started Y! Finance"
Rfunctions( self )
#print "Finished Y! Finance"
