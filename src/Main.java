import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class Main {
    public static void main(String[] args) {
        TeamStrengthEstimator estimator = new TeamStrengthEstimator();
        estimator.run();
        Map<String, Double> teamStrengths = estimator.getStrengthRatios();

        Map<String, Team> teamMap = new HashMap<>();
        Map<Team, Integer> teamIndex = new HashMap<>();
        for (String teamName : teamStrengths.keySet()) {
            Team team = new Team(teamIndex.size(), teamName, teamStrengths.get(teamName));
            teamMap.put(teamName, team);
            teamIndex.put(team, teamIndex.size());
        }

        ArrayList<MatchResult> matchResults = new ArrayList<>();
        matchResults.add(new MatchResult(teamMap.get("安邦一队"), teamMap.get("秉文"), 3, 0, 28+25+15, 26+17+8));
        matchResults.add(new MatchResult(teamMap.get("安邦一队"), teamMap.get("有训"), 2, 1, 25+25+13, 18+10+15));
        matchResults.add(new MatchResult(teamMap.get("安邦一队"), teamMap.get("毓琇"), 2, 1, 25+25+11, 16+18+15));
        matchResults.add(new MatchResult(teamMap.get("安邦一队"), teamMap.get("健雄一队"), 1, 2, 16+25+13, 25+21+15));
        matchResults.add(new MatchResult(teamMap.get("安邦一队"), teamMap.get("健雄二队"), 3, 0, 25+25+15, 8+18+8));

        matchResults.add(new MatchResult(teamMap.get("安邦二队"), teamMap.get("开甲"), 0, 3, 9+11+5, 25+25+15));
        matchResults.add(new MatchResult(teamMap.get("安邦二队"), teamMap.get("行知"), 0, 3, 13+13+5, 25+25+15));
        matchResults.add(new MatchResult(teamMap.get("安邦二队"), teamMap.get("有训"), 0, 3, 0, 0));
        matchResults.add(new MatchResult(teamMap.get("安邦二队"), teamMap.get("健雄一队"), 0, 3, 7+14+10, 25+25+15));

        matchResults.add(new MatchResult(teamMap.get("秉文"), teamMap.get("开甲"), 1, 2, 14+23+15, 25+25+11));
        matchResults.add(new MatchResult(teamMap.get("秉文"), teamMap.get("毓琇"), 1, 2, 26+23+9, 24+25+15));
        matchResults.add(new MatchResult(teamMap.get("秉文"), teamMap.get("健雄一队"), 0, 3, 15+15+9, 25+25+15));
        matchResults.add(new MatchResult(teamMap.get("秉文"), teamMap.get("健雄二队"), 3, 0, 25+25+15, 14+19+11));

        matchResults.add(new MatchResult(teamMap.get("开甲"), teamMap.get("行知"), 3, 0, 25+25+15, 13+19+12));
        matchResults.add(new MatchResult(teamMap.get("开甲"), teamMap.get("有训"), 2, 1, 25+25+6, 13+18+15));

        matchResults.add(new MatchResult(teamMap.get("行知"), teamMap.get("毓琇"), 1, 2, 13+25+11, 25+17+15));
        matchResults.add(new MatchResult(teamMap.get("行知"), teamMap.get("健雄一队"), 2, 1, 25+25+8, 16+22+15));
        matchResults.add(new MatchResult(teamMap.get("行知"), teamMap.get("健雄二队"), 2, 1, 25+23+15, 9+25+6));

        matchResults.add(new MatchResult(teamMap.get("有训"), teamMap.get("毓琇"), 0, 3, 17+14+13, 25+25+15));

        matchResults.add(new MatchResult(teamMap.get("毓琇"), teamMap.get("健雄二队"), 3, 0, 25+25+15, 11+11+5));

        VolleyballMonteCarloSimulation simulation = new VolleyballMonteCarloSimulation(matchResults, teamMap, teamIndex);
        simulation.run(100000000);
    }
}