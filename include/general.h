#ifndef GENERAL_H_
#define GENERAL_H_

#include<Eigen/Dense>
#include<complex>
#include<string>
#include<vector>
#include<ctime>
#include<iostream>
#include<fstream>
using namespace std;
using namespace Eigen;

static clock_t StartTime, EndTime; 

const double speedOfLight = 137.0359991;

typedef Matrix<MatrixXd,-1,1> vMatrixXd;
typedef Matrix<MatrixXcd,-1,1> vMatrixXcd;
typedef Matrix<VectorXd,-1,1> vVectorXd;
typedef Matrix<MatrixXd,-1,-1> mMatrixXd;


/*
    Read and write matrix
*/
template<typename T> void writeMatrixBinary(const Matrix<T,-1,-1>& inputM, const string& filename)
{
    ofstream ofs;
    ofs.open(filename, ios::binary);
        for(int ii = 0; ii < inputM.rows(); ii++)
        for(int jj = 0; jj < inputM.cols(); jj++)
        {
            ofs.write((char*) &inputM(ii,jj), sizeof(T));
        }
    ofs.close();
    return;
}
template<typename T> void readMatrixBinary(Matrix<T,-1,-1>& inputM, const string& filename)
{
    ifstream ifs;
    ifs.open(filename, ios::binary);
    if(!ifs)
    {
        cout << "ERROR opening file " << filename << endl;
        exit(99);
    }
        for(int ii = 0; ii < inputM.rows(); ii++)
        for(int jj = 0; jj < inputM.cols(); jj++)
        {
            ifs.read((char*) &inputM(ii,jj), sizeof(T));
        }
    ifs.close();
    return;
}
template<typename T> void writeMatrixBinary(T* inputM, const int& size, const string& filename)
{
    ofstream ofs;
    ofs.open(filename, ios::binary);
        for(int ii = 0; ii < size; ii++)
        {
            ofs.write((char*) &(inputM[ii]), sizeof(T));
        }
    ofs.close();
    return;
}
template<typename T> void readMatrixBinary(T* inputM, const int& size, const string& filename)
{
    ifstream ifs;
    inputM = new T[size];
    ifs.open(filename, ios::binary);
    if(!ifs)
    {
        cout << "ERROR opening file " << filename << endl;
        exit(99);
    }
        for(int ii = 0; ii < size; ii++)
        {
            ifs.read((char*) &(inputM[ii]), sizeof(T));
        }
    ifs.close();
    return;
}


/*
    gtos in form of angular shell
*/
struct intShell
{
    VectorXd exp_a, norm;
    MatrixXd coeff;
    int l;
};
/*
    Irreducible rep |j, l, m_j>
*/
struct irrep_jm
{
    int l, two_j, two_mj, size;
};
/*
    Coulomb and exchange integral in [Iirrep][Jirrep][eij][ekl]
*/
struct int2eJK
{
    double ****J, ****K;
};

/* factorial and double factorial functions */
double factorial(const int& n);
double double_factorial(const int& n);

/* transformation matrix for complex SH to solid SH */
complex<double> U_SH_trans(const int& mu, const int& mm);

/* evaluate "difference" between two MatrixXd */
double evaluateChange(const MatrixXd& M1, const MatrixXd& M2);
/* evaluate M^{-1/2} */
MatrixXd matrix_half_inverse(const MatrixXd& inputM);
/* evaluate M^{1/2} */
MatrixXd matrix_half(const MatrixXd& inputM);
/* solver for generalized eigen equation MC=SCE, s_h_i = S^{1/2} */
void eigensolverG(const MatrixXd& inputM, const MatrixXd& s_h_i, VectorXd& values, MatrixXd& vectors);
/* remove all spaces in a string) */
string removeSpaces(const string& flags);
/* Split function in python */
vector<string> stringSplit(const string& flags);
vector<string> stringSplit(const string& flags, const char delimiter);

/* Static functions used in X2C */
namespace X2C
{
    MatrixXd get_X(const MatrixXd& S_, const MatrixXd& T_, const MatrixXd& W_, const MatrixXd& V_);
    MatrixXd get_X(const MatrixXd& coeff);
    MatrixXd get_R(const MatrixXd& S_, const MatrixXd& T_, const MatrixXd& X_);
    MatrixXd get_R(const MatrixXd& S_4c, const MatrixXd& X_);
    MatrixXd evaluate_h1e_x2c(const MatrixXd& S_, const MatrixXd& T_, const MatrixXd& W_, const MatrixXd& V_);
    MatrixXd evaluate_h1e_x2c(const MatrixXd& S_, const MatrixXd& T_, const MatrixXd& W_, const MatrixXd& V_, const MatrixXd& X_, const MatrixXd& R_);
    MatrixXd transform_4c_2c(const MatrixXd& M_4c, const MatrixXd XXX, const MatrixXd& RRR);
}


/* Reoder and basis transformation */
namespace Rotate
{
    
    /* Generate basis transformation matrix */
    MatrixXd jspinor2sph(const Matrix<irrep_jm, Dynamic, 1>& irrep_list);
    MatrixXcd sph2solid(const Matrix<irrep_jm, Dynamic, 1>& irrep_list);
    /* For CFOUR interface */
    MatrixXd reorder_m_cfour(const int& LL);
    MatrixXcd jspinor2cfour_interface_old(const Matrix<irrep_jm, Dynamic, 1>& irrep_list);
    /*
        Put one-electron integrals in a single matrix and reorder them.
        The new ordering is to put the single uncontracted spinors together (separate)
    */
    template<typename T> Matrix<T,-1,-1> unite_irrep(const Matrix<Matrix<T,-1,-1>,-1,1>& inputM, const Matrix<irrep_jm, Dynamic, 1>& irrep_list)
    {
        int size_spinor = 0, size_irrep = irrep_list.rows(), Lmax = irrep_list(size_irrep - 1).l;
        if(inputM.rows() != size_irrep)
        {
            cout << "ERROR: the size of inputM is not equal to Nirrep." << endl;
            exit(99);
        }
        for(int ir = 0; ir < size_irrep; ir++)
        {
            size_spinor += irrep_list(ir).size;
        }
        Matrix<T,-1,-1> outputM = Matrix<T,-1,-1>::Zero(size_spinor,size_spinor);
        int i_output = 0;
        for(int ir = 0; ir < size_irrep; ir += 4*irrep_list(ir).l+2)
        {
            for(int ii = 0; ii < irrep_list(ir).size; ii++)
            for(int jj = 0; jj < irrep_list(ir).size; jj++)
            for(int mi = 0; mi < 4*irrep_list(ir).l+2; mi++)
            {
                outputM(i_output + ii*(4*irrep_list(ir).l+2) + mi, i_output + jj*(4*irrep_list(ir).l+2) + mi) = inputM(ir+mi)(ii,jj);
            }
            i_output += (4*irrep_list(ir).l+2) * irrep_list(ir).size;
        }

        return outputM;
    }
    /* 
        Transfer separate basis to m-compact basis 
        px,py,pz,px,py,pz to px,px,py,py,pz,pz
    */
    template<typename T> Matrix<T,-1,-1> separate2mCompact(const Matrix<T,-1,-1>& inputM, const Matrix<irrep_jm, Dynamic, 1>& irrep_list)
    {
        int size = inputM.rows(), size_nr = size/2, size_irrep = irrep_list.rows(), Lmax = irrep_list(size_irrep - 1).l;
        int Lsize[Lmax+1];
        for(int ir = 0; ir < size_irrep; ir++)
        {
            Lsize[irrep_list(ir).l] = irrep_list(ir).size;
        }
        Matrix<T,-1,-1> outputM = Matrix<T,-1,-1>::Zero(size,size);
        int int_tmp = 0;
        for(int ll = 0; ll <= Lmax; ll++)
        {
            for(int mm = 0; mm < 2*ll+1; mm++)
            for(int nn = 0; nn < 2*ll+1; nn++)
            for(int ii = 0; ii < Lsize[ll]; ii++)
            for(int jj = 0; jj < Lsize[ll]; jj++)
            {
                outputM(int_tmp+mm*Lsize[ll]+ii,int_tmp+nn*Lsize[ll]+jj) = inputM(int_tmp+ii*(2*ll+1)+mm,int_tmp+jj*(2*ll+1)+nn);
                outputM(size_nr+int_tmp+mm*Lsize[ll]+ii,int_tmp+nn*Lsize[ll]+jj) = inputM(size_nr+int_tmp+ii*(2*ll+1)+mm,int_tmp+jj*(2*ll+1)+nn);
                outputM(int_tmp+mm*Lsize[ll]+ii,size_nr+int_tmp+nn*Lsize[ll]+jj) = inputM(int_tmp+ii*(2*ll+1)+mm,size_nr+int_tmp+jj*(2*ll+1)+nn);
                outputM(size_nr+int_tmp+mm*Lsize[ll]+ii,size_nr+int_tmp+nn*Lsize[ll]+jj) = inputM(size_nr+int_tmp+ii*(2*ll+1)+mm,size_nr+int_tmp+jj*(2*ll+1)+nn);
            }
            int_tmp += (2*ll+1)*Lsize[ll];
        }
        return outputM;
    }
} // namespace Rotate

/* evaluate wigner-nj symbols */
namespace CG
{
    const long double sqrt_fact[192] =
    {
       1.e0,  // sqrt( 0! )
       1.e0,  // sqrt( 1! )
       1.41421356237309504880168872420969807856967187537694807317667973799073247846210703885038753432764157273501384623091229702492483606e0,  // sqrt( 2! )
       2.44948974278317809819728407470589139196594748065667012843269256725096037745731502653985943310464023481859460122661418912485886546e0,  // sqrt( 3! )
       4.89897948556635619639456814941178278393189496131334025686538513450192075491463005307971886620928046963718920245322837824971773092e0,  // sqrt( 4! )
       1.09544511501033222691393956560160426790548938999596650845378889946498655424544546760171687232774125152945365188570278448814401580e1,  // sqrt( 5! )
       2.68328157299974763569100840247753148252874203153383086912507669449262511076536587929729729005453872996340981138092854020510392126e1,  // sqrt( 6! )
       7.09929573971953925108079394987394045809860147695320838745566360297138695092678364411890303261973101370635076213710546684174683263e1,  // sqrt( 7! )
       2.00798406368178131514761286188444997454275688631681327954685970339300505422636082359395530871588368679695591728008012880363405201e2,  // sqrt( 8! )
       6.02395219104534394544283858565334992362827065895043983864057911017901516267908247078186592614765106039086775184024038641090215602e2,  // sqrt( 9! )
       1.90494094396650522516116334262026750651138661181936412986520081062476955272580421198748287826167139964284649640371817342893534152e3,  // sqrt( 10! )
       6.31797435892232788349259958133974288383063131583924258546794085344359054527790052280649907308946814427510871081351321777400143187e3,  // sqrt( 11! )
       2.18861051811417556296029916566970888541185501048512202961870498346932471901598291508493831281230380030064564804726353159337686449e4,  // sqrt( 12! )
       7.89114744508046814381843836386651213559535545984687032901708667626283333169477095734090253421766295885901832718722020006270131131e4,  // sqrt( 13! )
       2.95259701280076485821472064164108121567914065083624989894215400465765712411569881434513525336084631255035516745786892274965471052e5,  // sqrt( 14! )
       1.14353590586391295875124025935227536479679460692756746047203955024011764992022399613417439301299479023240587654724497736948543537e6,  // sqrt( 15! )
       4.57414362345565183500496103740910145918717842771026984188815820096047059968089598453669757205197916092962350618897990947794174147e6,  // sqrt( 16! )
       1.88596773062531480843762406007567730907136993750657633891888062608784997305802101481249227593348844249748239326600351441854082118e7,  // sqrt( 17! )
       8.00148342854498449532394581745401973226644898378557981029191098168715455825487649716033035282428621106662552781242323508768084262e7,  // sqrt( 18! )
       3.48776576634429394130956982418777524197644097177994878915391522656342779124124425079013355892983189066789782813111308029629840419e8,  // sqrt( 19! )
       1.55977626862849788646899402841280038406505190136133901387599527438057677798349466663280164940257911107001803005576672678661941929e9,  // sqrt( 20! )
       7.14779281818586568914492660841968751584499825352782829669096081190220826150399380821934490549995648521140853432639828350901248976e9,  // sqrt( 21! )
       3.35261200823717100758289563358894258035954746968812392969780450665112757491189297343902100727980847529553099144584615400940485814e10,  // sqrt( 22! )
       1.60785623545405876688777172799045561825675088524751895006829034829549075275566997555682463013840549587546737135362022260952000916e11,  // sqrt( 23! )
       7.87685471322938290082864090810655516340949451130409951768493079245806980026323232337170488420607021557691875987060575861927083579e11,  // sqrt( 24! )
       3.93842735661469145041432045405327758170474725565204975884246539622903490013161616168585244210303510778845937993530287930963541790e12,  // sqrt( 25! )
       2.00821179442459613193062397798803888440733807940177204359707396766630661510921708976315180852842940647967898190626932854790764264e13,  // sqrt( 26! )
       1.04349745809073977642050292995306805073242790545851567191749615785611343025658644269276375758890866136343912557302838599123765918e14,  // sqrt( 27! )
       5.52166953567228487485881686751795668120562970161008327875846545403747463500539058327972719317455348416223180402353057795605003145e14,  // sqrt( 28! )
       2.97351004601291064051502607826761849910368489242792706329097673131363327343233393272304708499976874402806275686804059324394306519e15,  // sqrt( 29! )
       1.62865852716949558430499533647826111144035385139710399606494531315243794300481402241747852568545026475047407499440874851376049508e16,  // sqrt( 30! )
       9.06798690679354852900721146741091748000635114641942820805724965607982569751948666855796738626943293417934394533497354307848876804e16,  // sqrt( 31! )
       5.12962802680363491254014488879691270574763602194146738599982649940274846704652287660613522896495085766159262118070569535715988637e17,  // sqrt( 32! )
       2.94674695534107347883928894792265342550692709619553377474953639913344933256709971197144071626318085146842063114273620784832776829e18,  // sqrt( 33! )
       1.71823397428756524063279065648803345265308384339425774330593562474028446249135875339617376566130509989382756521225951600439409920e19,  // sqrt( 34! )
       1.01652092779175702171244426939444604949283400577924568272864870164120670367282366384314974757826396376622085492100515669837048915e20,  // sqrt( 35! )
       6.09912556675054213027466561636667629695700403467547409637189220984724022203694198305889848546958378259732512952603094019022293492e20,  // sqrt( 36! )
       3.70995324650140908569074842060255846515913460140761284196747893636871250924994134747545348825935700809292413965321274250134269321e21,  // sqrt( 37! )
       2.28696877430935010079511797394982561792105944768991440666484176273126720685349073970673834087788492302282734429455606832144526680e22,  // sqrt( 38! )
       1.42821154179615294686593232731305231487284119175099726053657178393285530653839579273986503758219821238764319449410275071296405703e23,  // sqrt( 39! )
       9.03280290523322408635610174897062868113249499239322969463355471577955912220164671678530037197320005392852170553839660128740882553e23,  // sqrt( 40! )
       5.78381592144527081578360937207048352326452191391476100288594768321394552748733927164778345798414498164722975187876485503958316555e24,  // sqrt( 41! )
       3.74834112342097260530658059426926197765990917882790821806336539728088722663438007550683862722053812508738742754434904472078676569e25,  // sqrt( 42! )
       2.45795164849461258960674062719681166901858423925177097720047031682900602581915799766879572315141001643869716086015079603667165998e26,  // sqrt( 43! )
       1.63042067417843078822851956329730798505634879268153053679525563830360600962639729977657371847095184679073121311887337680249339043e27,  // sqrt( 44! )
       1.09371943781520219703066180075254491749879884936259119540911753121162478955926727265093895884563403120115738592968657222554391374e28,  // sqrt( 45! )
       7.41796613622095807276237421597226918583683156051587803062814669622884115526205360535778869465853949925537626531246345470058586119e28,  // sqrt( 46! )
       5.08550136674023695658451670185508414750402894486411390539696487740848048161312985788547912222439817613165593781004164732415162090e29,  // sqrt( 47! )
       3.52333869966202265350590057672147479020364107716800766296118547686517965263329600693174022459483709278593517539600350279270491560e30,  // sqrt( 48! )
       2.46633708976341585745413040370503235314254875401760536407282983380562575684330720485221815721638596495015462277720245195489344092e31,  // sqrt( 49! )
       1.74396368086360611696209329639024710585191326897558324047864272734613071193020331987139357202301412659063895025780598925125882526e32,  // sqrt( 50! )
       1.24543918088655869949356205769180440362155545208944842798892190395569946108930176877662714906152787939573743216050997951858441239e33,  // sqrt( 51! )
       8.98098965431671558896778170657207607812303058969656723773695303982493167737341162470832561240581932257751900897965244154853565955e33,  // sqrt( 52! )
       6.53825915979171443878164923175681775447848234489441462257543365605033923568223926630973944591634355486580305265354001470966959098e34,  // sqrt( 53! )
       4.80461962427038942460267525096444474581677890599787542513789928343751371738505274132876941971630070115613293787691302849711513331e35,  // sqrt( 54! )
       3.56320127885841946103335126785472146485744714251387835143642428069088118456216452341041341980147775180897944907753941136457339513e36,  // sqrt( 55! )
       2.66645567712059195190700971396129960002654547275755908385123460574504268782789652344165000902356699877944113483356785932788225533e37,  // sqrt( 56! )
       2.01312988912482288333668455069536465757226950852667105738329348542469315450569001315058759628473353921342038693056377628423577723e38,  // sqrt( 57! )
       1.53315404682076176941316470568960874437721764595849815710014025657879018141777255681999490257467967417190802413824637840834926355e39,  // sqrt( 58! )
       1.17763796875648432762110199697108580390098517587827219046591134058948922630871513464618046998824088696332055703501229903626942142e40,  // sqrt( 59! )
       9.12194448171078825946968575298182076761989758008729862707953808340679149191894511688039722336290490298776924851999592245674455579e40,  // sqrt( 60! )
       7.12446639319201784948673912308403605115342792152469863691590156505101801426401717004356180392839375586081577284428991031313594555e41,  // sqrt( 61! )
       5.60981044781264757536224880159561496878455858929022994349887708734003352864826326319288076049336314746831847435013622877039666677e42,  // sqrt( 62! )
       4.45264900413724511229659804359122976223890654563461713314064998840294547764464140572106946730721695447560552768205632331226607445e43,  // sqrt( 63! )
       3.56211920330979608983727843487298380979112523650769370651251999072235638211571312457685557384577356358048442214564505864981285956e44,  // sqrt( 64! )
       2.87187231472474602194272790194524073444870778643568289059275144121890409733927229724970429295333129199209255066768063657831830096e45,  // sqrt( 65! )
       2.33312009780346083238760578326488162175233825356704738400407672532565480165118869174939340640083319270358392322405900135196359422e46,  // sqrt( 66! )
       1.90974110596668796970008672429388554580114244205223927850882380218786156490563574141511711210097128779789816551891350387712364040e47,  // sqrt( 67! )
       1.57481285949690879440363779396009326275936094511907927348260649569426434296116674661461533575980079445185539914104318034956583789e48,  // sqrt( 68! )
       1.30813780783272719906613355787988488474742553038260618097529089998416110861625874524745492151977081485826212051956863615976655223e49,  // sqrt( 69! )
       1.09446661301155695857080695109221322834464193656741880373311701853493847209187445328924965990853089456619613183550277109601185491e50,  // sqrt( 70! )
       9.22213960297642814598347871007016379244405330655250073949588827263859733612762493240907300374414232086655564810903624534823974361e50,  // sqrt( 71! )
       7.82524494037637692535809689270459170451177213130681544379448898644247724748136956988230217527058150716051439996133543773657737700e51,  // sqrt( 72! )
       6.68589220786028253245905833563765234227034118740635263111037525994344909121505166921644503822403506385210871492666060646471483028e52,  // sqrt( 73! )
       5.75142194723999224356836312510183507170717503745407529739064535400791308668242750098851281002707102080243859755291622358496175594e53,  // sqrt( 74! )
       4.98087751419319666971328299194607842982793723237294186740096356436026463411564188879783765889153653310753734197040058679676463269e54,  // sqrt( 75! )
       4.34222834690444424005209872779546900339005702303132999338336502115401886851975218723734444907730923276611999403504701539992544399e55,  // sqrt( 76! )
       3.81028991060110634246276414878912279469899189376847948137380447928094380918180029769310193704850935718701638918014732603345596086e56,  // sqrt( 77! )
       3.36515693218106810945967727285604411129254944824118933734333986153569242358087878183913204795186029124275985148593429402214399837e57,  // sqrt( 78! )
       2.99101690580026232102005152875488621048360693671928601224920587103536774673783612592410129622639161893332995690749590686915397181e58,  // sqrt( 79! )
       2.67524684928818862621490012042605003730753817304274266583374275281196864081824195291421105018390289955133147004755840698437789901e59,  // sqrt( 80! )
       2.40772216435936976359341010838344503357678435573846839925036847753077177673641775762278994516551260959619832304280256628594010911e60,  // sqrt( 81! )
       2.18028515039038913058435920563318004580900822652445583756730412897448711876835314575646304847584006217474381385978086294582308837e61,  // sqrt( 82! )
       1.98633430462262788036763464177703883166690275063913206990548481884394889425413375669141670259265518581617742355858761606343959592e62,  // sqrt( 83! )
       1.82050546128413283235920381364504675611058333192571437017025709284900830827008726121704902949677472567728953624088551849447068081e63,  // sqrt( 84! )
       1.67842310350535579040289669063464837920254840947966373949734039701098864601738468609402017728262720929561430942955008591418008862e64,  // sqrt( 85! )
       1.55650555359345674201535001388480503193835670087005087695666582899389870112149114169661190154314330579536657495764985120826626875e65,  // sqrt( 86! )
       1.45181172966040184049837977571737270114599027103330879923163766760148991986710773409645029340223533539301334026515796804784908842e66,  // sqrt( 87! )
       1.36192012341913223936272532129340230424043887314619069524308123979942008271779741228004131650140685541231125195467149201818120290e67,  // sqrt( 88! )
       1.28483287477042947436606854413089420338280480054241478815100633956558896341672601184977044491448237914937935298242406730644851337e68,  // sqrt( 89! )
       1.21889948908093381697322725306802138223162932144898188493037149032868903716151236882884297469989275492948767356166562991390801476e69,  // sqrt( 90! )
       1.16275600522138906842396938125351518822885538657653139039959582739435860833119381883909133581666645175513110005314741781292475587e70,  // sqrt( 91! )
       1.11527638075238136262755547542307772029800712447380847367329167783382104209155198376527108391429797296628484222299440262828253178e71,  // sqrt( 92! )
       1.07553359179601711534343045655120043974654897757716293654507477655249875411094324741984133094791623508125145932166929137945922205e72,  // sqrt( 93! )
       1.04276850578483769255079421914426300125848286243632221011723235705793325995840829988147468547052449186112556554160413271026268467e73,  // sqrt( 94! )
       1.01636501751285493870798488028947073709983082260934656117721845292980988580746184325911703563740718894089918255299747699815527060e74,  // sqrt( 95! )
       9.95830274128553338795685900500337369492022050359737013283228306377142743139862329408774307125126208248844807817030677866543740137e74,  // sqrt( 96! )
       9.80779076461575621093405241807928914834646052755522060961382474180093668733435430580596247390782877622444296316930620145239443328e75,  // sqrt( 97! )
       9.70921750136603322844481600341927955944262662649446043656179790121056532220568311315320802030896597802018841703702405963515117513e76,  // sqrt( 98! )
       9.66054943799492973133000870362309068674974070396662776244736194062917963496762337871787018704439191692278092607403691295217541703e77,  // sqrt( 99! )
       9.66054943799492973133000870362309068674974070396662776244736194062917963496762337871787018704439191692278092607403691295217541703e78,  // sqrt( 100! )
       9.70873202835383618605273092198425567066140484064973931593038887370411056613418862432009712757240712335096311932278147414793959215e79,  // sqrt( 101! )
       9.80533870655936423948625541326149462775640364901662691744776797397063645922018558815077114052599266451060193513319667206310259115e80,  // sqrt( 102! )
       9.95133192918725850273223696018532306228626919601688243057779891670003196453149284181232692611730830343685537538660003825375666794e81,  // sqrt( 103! )
       1.01484071386329527885209097242248445236636861390059750092770501250120539741035912275772216714001597977673470842688197250555374199e83,  // sqrt( 104! )
       1.03990228302484791713044247966835568188068103714951362457574507672046074931625032571814273819961763140198650122608485513404995497e84,  // sqrt( 105! )
       1.07064492887918184662683362784780325423572507366000161338564673303244298243237783126323603970625679706627243293298593307211235962e85,  // sqrt( 106! )
       1.10748372592834877256996271322478239333596957884433122790811629506376644324689283182593589248103533798601399340893804189416497259e86,  // sqrt( 107! )
       1.15093084911815139620344474957114005614225151273873074888904642248430699494818555506706220132397159219211281172163008337196688916e87,  // sqrt( 108! )
       1.20160708353541823742653879375220696276694897230010902857965772723910925653836580777675256149408848857563553405831596186643647284e88,  // sqrt( 109! )
       1.26025614123587707293295876079117512537385342131235696615275004912895251195083773777278713316967756211100247112442148994715298837e89,  // sqrt( 110! )
       1.32776223439674795710251844143130447531824854281030772324528158650773709117459286696631702154599892486074107535565233824050788941e90,  // sqrt( 111! )
       1.40517146897489845936239625052390798683480525947808329256638474844702444913948511096144021241270255809805602696589451300569305836e91,  // sqrt( 112! )
       1.49371776070977131787360943527332592927665828376130329195744662120431138890413125151274108025647452378894420861878838041613180736e92,  // sqrt( 113! )
       1.59485414175472095336601953455366856705187839970896498140875912020912642633230781605792672927867169618648294850951644083811611041e93,  // sqrt( 114! )
       1.71029052897249468768104113807320628912502596232127991403391055132584872814784687440888705398361187012936729053521216240433326764e94,  // sqrt( 115! )
       1.84203927331962664677022195613714175289873806750604681693578915410549137313659497763356116645156026990141752216533524435323377767e95,  // sqrt( 116! )
       1.99247011541170195173352979812132162098852155263505713991367432578710986646624282509957654502380698046657409367566519175600988970e96,  // sqrt( 117! )
       2.16437654989936782408869323777781180807307911095712192214658419412705096262371684368082522996871048540705060828729594421454061830e97,  // sqrt( 118! )
       2.36105606905206846282664890558362403647951978515981481952412322048434952117837438819546766063940584395116075057199945771732452664e98,  // sqrt( 119! )
       2.58640733710858604531099177067755353124510283494480561802616291158190362737407307300074705863090397129498902451840197871922406891e99,  // sqrt( 120! )
       2.84504807081944464984209094774530888436961311843928617982877920274009399011148038030082176449399436842448792697024217659114647580e100,  // sqrt( 121! )
       3.14245830534529150853639626024900715446450496615794609384389709639185275103382116559145270275525398121619520027233807683558908324e101,  // sqrt( 122! )
       3.48515485553014267718746632044644135798112777888235237194719289716564641629221139688887779942966340391994606707178478820352659208e102,  // sqrt( 123! )
       3.88090420071294838937815654068656208347987977102403596992139813557831564016869902866336970309069864349224411954727887152552750091e103,  // sqrt( 124! )
       4.33898280347932019750481702625682793859075389113838895819077693640544086702009849212692471607705240138022684596510356066044365041e104,  // sqrt( 125! )
       4.87049611731705098346051630013863844593606663992166576311186486403644643566965859972514168255024757791633813692231248997491735255e105,  // sqrt( 126! )
       5.48877037090973550304238288709596895379120823255704639562377299888870762892829892881768872031382026869498567744947110793570012708e106,  // sqrt( 127! )
       6.20983479943372102050500882162988260350351194250463657930138001878476335551284905635722956448403671923272107515696882441487834722e107,  // sqrt( 128! )
       7.05301653370902529914710641082344999408759672508302572697332940865487899030288553602637994056904404806389344552620323464389071210e108,  // sqrt( 129! )
       8.04167612455293655275585835841548748736806208579440801018024055314507932999785930774705665068919831417860134569461059375300141006e109,  // sqrt( 130! )
       9.20411901861271050295051027235109192713260449490524894719849952957721563028226802209569651348762810430819566655890980553792457321e110,  // sqrt( 131! )
       1.05747276617225670633502191923057952746107397228730290270644104224488813794803215696564809964411459163795363515324691403589750272e112,  // sqrt( 132! )
       1.21953708680412246366972638116354683210692919041731538335203296601774788492007266000185472804948064523527708686363050682393716785e113,  // sqrt( 133! )
       1.41171624137484473326138337602689335092779395618009137821694128257714357864465447701963109023908659187924466816994624089875987559e114,  // sqrt( 134! )
       1.64026604772459107424003011057835411433113164410351852341644398752863538944179223780675697125810676402066087556353909455525561744e115,  // sqrt( 135! )
       1.91286248380602327544317614701311153829489188058551938933232891978200393441706467817780853408389322225233074235305621132938893608e116,  // sqrt( 136! )
       2.23894813434232809631438692509391146802442630581410697063247802423727482122138201980379524879793911448098618378993491922991987935e117,  // sqrt( 137! )
       2.63016852551685145668775403941742802756503599021110217063699156719658289899690656856406083443965578580268438074410469152196358000e118,  // sqrt( 138! )
       3.10092295888515892103206668865055255699981302077534368600155843030927279835722383750028506599529595435985925743636819744406585219e119,  // sqrt( 139! )
       3.66906152520198611698604256404320124966288705895134462474158705612402587219159405223134464197337514370440919893192744662510418961e120,  // sqrt( 140! )
       4.35676916886374755917975274919576568436258675595063799973277246266506571797330509986832571899627518721459278808225618128609377893e121,  // sqrt( 141! )
       5.19168964585534790669805677866941861724525489636345037042525780604373672243586793289412792839959190577891507749791445310426963889e122,  // sqrt( 142! )
       6.20835784823980065856846151703040354154034866972351145951013109237760158313256044057434163388919092363163202357384410340129704388e123,  // sqrt( 143! )
       7.45002941788776079028215382043648424984841840366821375141215731085312189975907252868920996066702910835795842828861292408155645266e124,  // sqrt( 144! )
       8.97102338502803811794374820046926007295111348643549607832559709145227808897672222303246240972613115479519100334524335032445013037e125,  // sqrt( 145! )
       1.08397287991485784401760544272712189961917205241985237925381216163452185236118916492348837646513744152428583821209830913629303361e127,  // sqrt( 146! )
       1.31424727142750382474286269190923203841500326384006987785611252350912451307453702011697956519506894801186429400911741017331695305e128,  // sqrt( 147! )
       1.59885081163717885782627317281198286296350774286305718641752540915043226472996064938360981347548529940818754755600868921510163852e129,  // sqrt( 148! )
       1.95164613535101944899010613671982385560223037637465372819975714454728364776768419168479320105137761050984093318638637644750491812e130,  // sqrt( 149! )
       2.39026859504237610909102395888679607568840426470595308508788933821499178187866901887662192081798769842566729270704183990506171509e131,  // sqrt( 150! )
       2.93721122397304624483868275617533444861566279614533565188564932095281558165870715549839880335234508470547489104031516628960999801e132,  // sqrt( 151! )
       3.62123719974741856954350426607455064768812934939944813814442284351457474334181373624816704288504270865388614400869795613496837810e133,  // sqrt( 152! )
       4.47922304099235763294320277900590271557394355758549178403300790651278672352622531414876243040586448295734384083122798745658940804e134,  // sqrt( 153! )
       5.55856961263178828596691236915891983780688018103885996827724710510434837498033483780052128057748547064236846679439367777227164152e135,  // sqrt( 154! )
       6.92036335856968848246372726254025133928746306449363702312731791859427589772130627326938719874076061983152929851313001770632663379e136,  // sqrt( 155! )
       8.64353106449146421246647564875621885932209128058697353200406994622817018428979186816339028675511266863819570328222119051052393952e137,  // sqrt( 156! )
       1.08303133815527906366456705610986247201600052164737096326673262454928854703397743872065960728483358193173863517144212580964540944e139,  // sqrt( 157! )
       1.36134928269483243465684570758084065663363648273732059723871902964209713421115441816127930767563582313110068231835317485301599340e140,  // sqrt( 158! )
       1.71659612969825791999580332984496354370542214737412523452590212717168619017700185030002191664066790335476049884403007064474984337e141,  // sqrt( 159! )
       2.17134143699052114838050994260418553192933091722445682927098711962936332910813890021043455944478936880473379387400076818877452661e142,  // sqrt( 160! )
       2.75512341900453140645224177474357530163091238945368028614199663374232663440231350339138734173462608133623327214613489349651612663e143,  // sqrt( 161! )
       3.50669961465114578483507918334761172531097326960848239324762107500301227555384521401903055873187749467910206594112395516292011769e144,  // sqrt( 162! )
       4.47705436257513247721260740501935606183383762232044065589583071913636015113770075697896223123427687241274750204330928918465274524e145,  // sqrt( 163! )
       5.73342706026186071970057748322687838569048868177958833956127174959466308251648266359214123562674844828421194147242737273027004975e146,  // sqrt( 164! )
       7.36472040618758912501000655518343370572168406372244451125759615814278111418751279863971452825686718694692150146386028183930898136e147,  // sqrt( 165! )
       9.48877848080480660900068264413956457614594149453126673899067299594531091571018059188073133529505564454500476150213132115330437679e148,  // sqrt( 166! )
       1.22622041854839419412622499476628853173820557585362424077271230416751792575802561025677855979066648430158970349058436680756187683e150,  // sqrt( 167! )
       1.58936331434105977590901443772353046830062240292885070519563276463500840305205985926401140220708389734826105353627909415245311180e151,  // sqrt( 168! )
       2.06617230864337770868171876904058960879080912380750591675432259402551092396767781704321482286920906655273936959716282239818904534e152,  // sqrt( 169! )
       2.69395909681420348164295232892611033628073540891751377058242790609099763125835133593681629558187160635413923054175725080616367356e153,  // sqrt( 170! )
       3.52280863831356559739281555848772825491118343527476600401195961019143430697983174682554670973902853225075269312610879440118670109e154,  // sqrt( 171! )
       4.62012021572424958939977707342271428207953534175231687445940165881059776285530033117056713105739006506722295654218700689346055988e155,  // sqrt( 172! )
       6.07681937343845392610432233674157520923743581364207099448538266921916330563613387585211890738249706555235458498773046166302000143e156,  // sqrt( 173! )
       8.01587528804376093454895515122666268750066994017148052424651984068367861642238231659954781608718786580212526915068408233818821871e157,  // sqrt( 174! )
       1.06040062763360166180977695366550046739609545476221776312716130351435396932218081508541286157222700361086515287429677945756297146e159,  // sqrt( 175! )
       1.40678040372721141264234580517290705563464155497783139916191855064184482919533713863323446334109975110095137446935839595354169823e160,  // sqrt( 176! )
       1.87159959783878074275664907253097650412623788672348368711555345413941599322478565003995743920927065769990950067494704379111009826e161,  // sqrt( 177! )
       2.49702530969189592267003223076265810848204001537664345657622012874000809901150850713088944507817741713922354408269688267633501993e162,  // sqrt( 178! )
       3.34079217567675356401815270341668410283499165263864178213474158608971470964897056424701374952536951156581017535535595293378259559e163,  // sqrt( 179! )
       4.48214304210758427304154289349438467104175126625221949805926263336779326155911230476038183963317562557354163430843982502721423439e164,  // sqrt( 180! )
       6.03010674137227095220747820563040969574216577898679028005920250891999972986247076490819262462315505557883856874299136788271740946e165,  // sqrt( 181! )
       8.13505875261296567072520034380234500280866281461346575958231238543998049111325619934221025310897943180817631595877322672707011237e166,  // sqrt( 182! )
       1.10049035008259214820236669003745430840266612247201755634970489412348787033548010628831137502995437157163017774445782799300327098e168,  // sqrt( 183! )
       1.49277773950103752626532791651829506497813226874354612042226474241501441325183348719531673747596690849273022687766703217508698903e169,  // sqrt( 184! )
       2.03039723999201220030240045513462039012980282624467612268520918998418986177384322082205376192115830711911756592544176049073573402e170,  // sqrt( 185! )
       2.76909264760696590600598011883855959141913427623500245919113063478528245757450178611180055258532055220650248998485166513787043063e171,  // sqrt( 186! )
       3.78667724400005977240664338903803330433699240857338889775580368710132072312286168402587545217971378914285213527648662774979013965e172,  // sqrt( 187! )
       5.19203025361259136814182783350992047812208924278897481680746889547955218433627690319266899480947926151756647633259198115202347024e173,  // sqrt( 188! )
       7.13786149430414013834580375498898333380024599371583648711360815058118094749580298487270894177692780854696831958961902107711302974e174,  // sqrt( 189! )
       9.83886308231558284658391349556008112423630838894810393159236612543130058031928558846316170559678458055061516070556017547545279558e175,  // sqrt( 190! )
       1.35975793102072127197584619684017539710327446682830024041110759004397440380130651896772216452518306764491453984964618714830010495e177   // sqrt( 191! )
    };
    bool triangle_fails(const int two_j1, const int two_j2, const int two_j3);
    double sqrt_delta(const int two_j1, const int two_j2, const int two_j3);
    double wigner_3j_int(const int& l1, const int& l2, const int& l3, const int& m1, const int& m2, const int& m3);
    double wigner_3j_zeroM(const int& l1, const int& l2, const int& l3);
    double wigner_3j(const int& tj1, const int& tj2, const int& tj3, const int& tm1, const int& tm2, const int& tm3);
    double wigner_6j(const int& tj1, const int& tj2, const int& tj3, const int& tj4, const int& tj5, const int& tj6);
    double wigner_9j(const int& tj1, const int& tj2, const int& tj3, const int& tj4, const int& tj5, const int& tj6, const int& tj7, const int& tj8, const int& tj9);
}

#endif