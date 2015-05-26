(function(){
    var Host=function(){
        var o=function(){return "called o!"};
        o.__proto__=Host.prototype;
        return o;
    }
    Host.prototype=Object.create(Function.prototype);
    Host._native={};
    Host.prototype.extract=function(a){
        return a;
    }
    Host.prototype.x=Host.prototype.extract;
    Host.prototype.log=function(str){
        Host._native.log(str);
    }
    return Host;
})();
