#include "../logic/HorcruxData.hpp"
#include "../networking/Requests.hpp"

#include <array>
#include <asio.hpp>
#include <iostream>

using asio::ip::tcp;
using namespace Horcrux;

std::vector<HorcruxData> getSampleHorcrux()
{
    std::string binaries =
        "Allora signori, siccome non mi lascio prendere per il culo da semplici ragazzetti, che escono FORSE dall’università (in realtà saranno sì e no al secondo, terzo anno del politecnico) e "
        "fanno tanto gli spavaldi cercando di dimostrare cose, che non sanno NEANCHE di che cosa si tratta, partendo da teorie in parte esatte ma ovviamente mancando di completarle per i propri "
        "comodi, volevo farvi vedere un qualcosa, visto che questi blahblahblah ci sono. Su questo computer… su questa MACCHINA che state vedendo ci sono i file che ho registrato e ho mandato, va "
        "bene? Su Last. I file a 13 kHz, i file a 12 kHz, i file a 6, i file ad 1 kHz sinusoidale. Qualcuno ha pensato di cercare di mettermi in ridicolo… uhm, semplicemente facendo vedere che "
        "queste cose, che sono state DIGITALIZZATE, dovevano apparire come MERDA alle orecchie dell’ascoltatore in quanto secondo il teorema del campionamento di Nyquist-Shannon ed altri, ok, queste "
        "povere sinusoidi venivano torturate, distrutte e mai più ricostruite secondo… oh… secondo quello che erano, quindi, mai più ricostruite fedelmente. Questa gente è solo DEMENTE. Adesso vi "
        "mostro quest’altra macchina, che sta captando la mia voce… questo è un semplice programma di oscilloscopio, vedete, fa l’analisi delle ampiezze e c’è anche l’analisi di spettro, ma quello "
        "che mi interessa adesso è l’analisi delle ampiezze, vedete… le onde che la mia voce produce vengono captate dal microfono che sta dentro, ok, a questo laptop e… diciamo che adesso simuliamo "
        "l’orecchio umano, ossia, questo laptop è l’orecchio, il VOSTRO orecchio, va bene? l’orecchio è collegato al cervello per mezzo del nervo acustico, tipicamente, l’orecchio interno. Nella "
        "testa di questi “signori”, l’orecchio è collegato al niente, quindi hanno il nervo a penzoloni. Vi mostro che cosa succede quando io mando, per esempio, il segnale da 1 kHz, partiamo da "
        "questo… ecco, e lo mandiamo in play. Allora, andiamo sull’altro monitor e vediamo che sta saturando perché il segnale è fortissimo, ovviamente… ‘sto povero disgraziato, dietro "
        "all’altoparlante, ce l’ho messo proprio dietro… abbasso il volume. MI SEMBRA, aldilà della mia voce che si sovrappone, DI VEDERE una sinusoide… vediamo, queste linee sono dovute al rumore "
        "di fondo e non c’entrano niente con il segnale: mentre stavo parlando, le avete viste… ecco. Olà. Queste interferenze non sono dovute all’audio… intendo dire al segnale ma solo al fatto che "
        "non è interconnesso via cavo. Saturava lo stadio. Bene. Rivediamolo. OH MA CHE BEL CASTELLO MARCONDIRONDIRONDELLO, direbbe qualcuno. Adesso, vi faccio vedere… questo è in elaborazione… di "
        "windows media player… andiamo sul segnale da 6 kHz, che mi hanno rappresentato come fosse una schifezza… velo faccio vedere subito come schifezza questo segnale da 6 Khz. È una sinusoide a "
        "casa mia. Ok, ho amplificato la scala. Come vedete è molto diverso dal segnale che loro hanno rappresentato nelle loro figure, stranamente sinusoidale. Attenzione, 6 kHz campionato a 44100 "
        "Hz, ovvero 44,1 kHz. Siccome qualcuno voleva fare tanto il gradasso, in olandese è chiamato DIKKE NEK, cioè “collo grosso”, “pallone gonfiato”, spiegando, uhm, in un certo modo più o meno "
        "approfondito, e risibile in quanto veramente incompleto, schifoso, COSA CHE SE QUELL’IMBECILLE ha superato un esame, se le leggi in Italia fossero come qui in Belgio, con questa "
        "dimostrazione potrebbe essere annullato quell’esame… andiamo a vedere cosa succede, invece, sul segnale a 12 kHz… lo evidenzio… anche questo è una sinusoide, che loro… con lo zoom non "
        "riesco così vicino… a mettere a fuoco… eccolo qua, 12 kHz… che loro, o meglio un certo, com’era il nome? Andrea C.? Ah, sta per ANDREA COGLIONE probabilmente, sì sì, o ANDREA CIACULLI, "
        "dipenda da che zona di palermo arriva, ‘st’imbecille… Vabè, comunque, dicevo… questi IMBECILLI, perché è la loro categoria, cosa ci volete fare… esistono gli esseri umani, esistono gli "
        "animali, esistono i vegetali, esistono gli imbecilli. Costui, è venuto a rappresentare, in una forma, insieme ad un altro, uhm, diciamo, molto inventata… perché hanno inventato quelle "
        "immagini che vi hanno proposto, ok, questo segnale. Allora, lo apriamo… e andiamo a vedere se è una sinusoide o se è la porcheria che loro hanno rappresentato nelle loro immagini… Mmh. Una "
        "sinusoide, wow. Alzo il volume… il segnale è finito perché era corto, adesso, il file durava poco… mi sembra più che perfetta. Amplifico la scala… Ecco la vostra sinusoide.\n"
        "";

    Horcrux::Client::BasicFileSplitter splitter(10);
    return splitter.split(binaries);
}

bool readResponse(tcp::socket& socket)
{
    Utils::log("Reading server response");
    std::array<char, 128> readBuf{};
    asio::error_code error;

    size_t len = socket.read_some(asio::buffer(readBuf), error);
    Utils::log("Received message with length: ", len);

    if (error)
        throw asio::system_error(error); // Some other error.

    std::string response(readBuf.data(), readBuf.data() + len);
    Utils::log("Received response is: ", response);
    if (response == "SUCCESS")
        return true;

    if (response == "FAILURE")
        return false;

    return false;
}

bool sendHorcruxRequest(tcp::socket& socket, const Save::Request& request)
{
    std::error_code ec;
    auto serializedHeader = Utils::serializeHeader(request.mHeader);
    Utils::log("Sending request header: ", request.mHeader);
    size_t len = socket.write_some(asio::buffer(serializedHeader), ec);
    if (ec)
    {
        std::cout << ec.message() << std::endl;
        return false;
    }

    Utils::log("Written header on socket with size: ", len);

    len = socket.write_some(asio::buffer(request.mContent), ec);
    if (ec)
    {
        std::cout << ec.message() << std::endl;
        return false;
    }

    if (!readResponse(socket))
        return false;

    Utils::log("Written Horcrux content on socket with size: ", len);
    return true;
}

int main()
{
    try
    {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", std::to_string(8080));
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        std::error_code ec;

        std::vector<HorcruxData> horcruxes = getSampleHorcrux();
        Save::PreRequestHeader preRequestHeader = Save::PreRequestHeader::fromHorcruxes(1200, horcruxes);
        auto serializedPreRequestHeader = Utils::serializeHeader(preRequestHeader);

        size_t len = socket.write_some(asio::buffer(serializedPreRequestHeader, serializedPreRequestHeader.size()), ec);
        if (ec)
        {
            std::cout << ec.message() << std::endl;
            return 0;
        }
        Utils::log("Sent N bytes to socket: ", len);

        if (!readResponse(socket))
            return 0;

        auto requests = Save::Request::fromHorcruxes(preRequestHeader.mUuid, std::move(horcruxes));
        for (auto& request : requests)
        {
            Utils::log("#### Sending Horcrux with id: ", request.mHeader.mIndex);
            bool ok = sendHorcruxRequest(socket, request);
            if (!ok)
                Utils::log("Something went wrong while shipping stuff");
            Utils::log("#### End message");
        }

    } catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}