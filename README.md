QWarpIN
=======
(*intro to be written*)

(*read  "intro" as WTF-is-this*)

(*better name ideas are welcome*)

Building on OS/2
----------------
Assuming you have got a fresh eComStation 2.2 beta 2 installed<sup>[1](#warp)</sup>, the following will set up the required environment that will kindly meet all dependencies, download and build QWarpIN for you:
```
#!/@unixroot/usr/bin/sh
export COMSPEC=sh
export SHELL=sh
mkdir QWarpIN
cd QWarpIN
rm -rf *
yum -y update
yum -y install bzip2-devel bzip2-libs curl gcc kbuild-make libqt4-devel qt4-devel-tools git subversion
curl -O ftp://dbanet.org/os2/rpm/{bisonc++,flexc++,cmake,ilink,libbobcat,libbobcat-dev}.rpm
yum -y localinstall *
rm -rf *
svn export http://svn.netlabs.org/repos/ports/boost/trunk boost
cd boost
./bootstrap.sh --prefix=/@unixroot/usr
./b2 install --with-program_options link=static
cd ..
git clone https://github.com/dbanet/QWarpIN.git qwp
mkdir qwp-bld
cd qwp-bld
cmake ../qwp
make
cd ../..
exit
```
If your environment is not screwed up, this will run absolutely unattended, so you can even write this to a file and then run it with ``sh``.

You will essentially find the kindly generated binary ``QWarpIN\qwp-bld\out\wpi.exe``.

---

<ol><li><a name="warp">All systems since Warp 4 are very likely to meet all the requirements, especially if well fixed with all IBM-supplied fixpacks, but in addition you will need to set up <a href="http://trac.netlabs.org/rpm/wiki/RpmInstall">a netlabs.org RPM/YUM environment</a>, which is bundled with eCS 2.2 beta 2, and proceed to the main instruction; if you do not like this approach, you are very free to set up your very own environment, but I most likely am not going to support this.</a></li></ol>

Licensing
---------
This is alpha. Alpha means there is no general availability releases, neither binary nor source. If you want to test or participate in software development, you are welcome to reach me, so I will be able to provide you with an alpha-release under required conditions, advice, coordination and support.

When the project reaches some code level at which it will be able to fit purposes and expectations and serve usefully, it will be beta-released for general availability under the GNU General Public License.

>                                         QWarpIN                                     
>                             Copyright (C) 2014, 2015 dbanet                         
>     
>                          LIMITED USE SOFTWARE LICENSE AGREEMENT                     
>     
>     This  Limited  Use Software  License  Agreement  (the  "Agreement") is  a  legal
>     agreement  between you,  the end-user,  and  the software  author, dbanet  ("the
>     Author"). By  downloading or  purchasing the  software material,  which includes
>     source code (the "Source Code"),  artwork data and software tools (collectively,
>     the "Software"), you are agreeing to be bound by the terms of this Agreement. If
>     you do not agree  to the terms of this Agreement,  promptly destroy the Software
>     you may have downloaded or copied.
>     
>     1. Grant of License.  The Author  grants to you  the right to use  the Software.
>        You have  no ownership or  proprietary rights in or  to the Software,  or the
>        Trademark. For  purposes of  this section, "use"  means loading  the Software
>        into RAM, as well as installation on a hard disk or other storage device. The
>        Software, together with any archive copy  thereof, shall be destroyed when no
>        longer used in accordance  with this Agreement, or when the  right to use the
>        Software is terminated.  You agree that you will not  utilize, in any manner,
>        the Software in violation of any applicable law.
>     
>     2. Permitted Uses.  For educational  purposes only, you,  the end-user,  may use
>        portions of  the Source Code,  such as  particular routines, to  develop your
>        own  software,  but may  not  duplicate  the  Source  Code, except  as  noted
>        in  paragraph 4. The  limited  right referenced  in  the  preceding  sentence
>        is  hereinafter  referred to  as  "Educational  Use".  By so  exercising  the
>        Educational  Use  right  you  shall  not  obtain  any  ownership,  copyright,
>        proprietary or other interest in or to the Source Code, or any portion of the
>        Source Code.  You may dispose of  your own software in  your sole discretion.
>        With the  exception of the Educational  Use right, you may  not otherwise use
>        the Software, or an portion of  the Software, which includes the Source Code,
>        for commercial gain.
>     
>     3. Prohibited  Uses:  Under   no  circumstances  shall  you,  the  end-user,  be
>        permitted,  allowed  or  authorized  to commercially  exploit  the  Software.
>        Neither you nor anyone  at your direction shall do any  of the following acts
>        with regard to the Software, or any portion thereof:
>     
>                    Rent;
>     
>                    Sell;
>     
>                    Lease;
>     
>                    Offer on a pay-per-use basis;
>     
>                    Distribute for money or any other consideration; or
>     
>                    In any other  manner and through any medium whatsoever
>                    commercially exploit or use for any commercial purpose.
>     
>        Notwithstanding the foregoing prohibitions,  you may commercially exploit the
>        software you develop  by exercising the Educational Use  right, referenced in
>        paragraph 2 hereinabove.
>     
>     4. Copyright.  The Software  and all  copyrights  related thereto  are owned  by
>        the  Author and  are protected  by  copyright laws  and international  treaty
>        provisions. The Author shall retain  exclusive ownership and copyright in and
>        to the  Software  and all  portions of  the Software  and you  shall have  no
>        ownership or  other proprietary  interest in such  materials. You  must treat
>        the  Software like  any other  copyrighted  material. You  may not  otherwise
>        reproduce, copy or disclose to others, in whole or in any part, the Software.
>        You may not  copy the written materials accompanying the  Software. You agree
>        to  use your  best efforts  to see  that any  user of  the Software  licensed
>        hereunder complies with this Agreement.
>     
>     5. Exclusive Remedies. The Software is being  offered to you free of any charge.
>        You  agree  that you  have  no remedy  against  the  Author, its  affiliates,
>        contractors,  suppliers,  and  agents  for  loss  or  damage  caused  by  any
>        defect  or  failure  in  the  Software regardless  of  the  form  of  action,
>        whether in  contract, tort, negligence,  strict liability or  otherwise, with
>        regard  to the  Software. Copyright  and  other proprietary  matters will  be
>        governed by laws and international treaties.
>     
>        NO  WARRANTIES. THE  SOFTWARE IS  PROVIDED  TO YOU  BY THE  AUTHOR ``AS  IS''
>        AND  ANY  EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT  LIMITED  TO,
>        THE  IMPLIED WARRANTIES  OF PRECISION,  COMPLETENESS, RELIABILITY,  ACCURACY,
>        QUALITY,  VALIDITY, STABILITY,  COMPLETENESS, CURRENTNESS,  OR OTHERWISE  ARE
>        DISCLAIMED. THERE IS  NO WARRANTY EVEN FOR MERCHANTABILITY AND  FITNESS FOR A
>        PARTICULAR PURPOSE. THE AUTHOR NEITHER DOES NOT WARRANT OR GUARANTEE THAT THE
>        OPERATION OF  THE SOFTWARE  WILL BE  UNINTERRUPTED, ERROR  FREE OR  MEET YOUR
>        SPECIFIC REQUIREMENTS,  NOR THE AUTHOR  DOES NOT WARRANT, GUARANTEE,  OR MAKE
>        ANY  REPRESENTATIONS REGARDING  THE USE  OR  THE RESULTS  OF THE  USE OF  THE
>        SOFTWARE,  HENCE IN  NO EVENT  SHALL  THE AUTHOR  BE LIABLE  FOR ANY  DIRECT,
>        INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES INCLUDING,
>        BUT NOT LIMITED TO  LOSS OF DATA, LOSS OF PROFITS,  LOSS OF SAVINGS, SPECIAL,
>        INCIDENTAL,  CONSEQUENTIAL, INDIRECT  OR OTHER  SIMILAR DAMAGES,  PROCUREMENT
>        OF  SUBSTITUTE  GOODS OR  SERVICES,  LOSS  OF  USE  OR PROFITS,  OR  BUSINESS
>        INTERRUPTION  HOWEVER CAUSED  ARISING IN  ANY WAY  OUT OF  AVAILABILITY, USE,
>        RELIANCE  ON, OR  INABILITY TO  USE THE  SOFTWARE, FROM  BREACH OF  WARRANTY,
>        BREACH OF CONTRACT, NEGLIGENCE, OR FROM  ANY OTHER LEGAL THEORY OF LIABILITY,
>        WHETHER  IN CONTRACT,  STRICT  LIABILITY, OR  TORT  (INCLUDING NEGLIGENCE  OR
>        OTHERWISE) EVEN IF  ADVISED OF THE POSSIBILITY OF SUCH  DAMAGES BY THE AUTHOR
>        OR BY ANY CLAIM  BY ANY OTHER PARTY. THE WARRANTY SET FORTH  ABOVE IS IN LIEU
>        OF  ALL  OTHER  EXPRESS  WARRANTIES  WHETHER ORAL  OR  WRITTEN.  THE  AGENTS,
>        EMPLOYEES, DISTRIBUTORS, AND DEALERS OF THE AUTHOR ARE NOT AUTHORIZED TO MAKE
>        MODIFICATIONS TO  THIS WARRANTY,  OR ADDITIONAL WARRANTIES  ON BEHALF  OF THE
>        AUTHOR.
>     
>        Some jurisdictions do not allow  the above disclaimer, so the license may not
>        apply to you.
